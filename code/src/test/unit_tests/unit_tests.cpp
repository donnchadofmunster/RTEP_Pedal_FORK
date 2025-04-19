#include <gtest/gtest.h>
#include "MockSamplingModule.h"
#include "MockOutputModule.h"
#include "DigitalSignalChain.h"
#include "Sample.h"
#include "EffectFactory.h"
#include "Config.h"

extern void ForceAllEffects();
const std::string ASSET_PATH = "../../../../assets";

class DSPTest : public ::testing::Test
{
protected:
    std::unique_ptr<DigitalSignalChain> chain;
    Config *config;

    void SetUp() override
    {
        ForceAllEffects();

        chain = std::make_unique<DigitalSignalChain>();

        config = &Config::getInstance();
        config->set("gain", true, 50.0f);      // enable Gain
        config->set("fuzz", false, 5.0f);       // disable Fuzz
        config->set("harmonizer", false, 0.0f); // disable Harmonizer for now

        chain->configureEffects(*config);
    }

};

// --- Sample core behaviour (no DSP chain) ---

TEST(SampleUnitTest, ConstructorInitialisesCorrectly)
{
    Sample s(0.75f);
    std::cout << "[unit_test] Sample made with value: " << s.getPcmValue() << "\n";
    EXPECT_FLOAT_EQ(s.getPcmValue(), 0.75f);
    EXPECT_TRUE(s.getAppliedEffects().empty());
}

TEST(SampleUnitTest, PcmValueSetterGetter)
{
    Sample s(0.0f);
    s.setPcmValue(1.23f);
    EXPECT_FLOAT_EQ(s.getPcmValue(), 1.23f);
}

TEST(SampleUnitTest, AppliedEffectCanBeAddedAndRetrieved)
{
    Sample s(0.0f);
    s.addEffect("Gain");
    s.addEffect("Fuzz");

    const auto &effects = s.getAppliedEffects();
    ASSERT_EQ(effects.size(), 2);
    EXPECT_EQ(effects[0], "Gain");
    EXPECT_EQ(effects[1], "Fuzz");
}

// --- Sample processing tests ---

TEST_F(DSPTest, SampleEffectListIsNotEmpty)
{
    Sample s1(0.5f);
    chain->applyEffects(s1);
    EXPECT_FALSE(s1.getAppliedEffects().empty());
}

TEST_F(DSPTest, SamplePCMIsTransformed)
{
    Sample s2(0.5f);
    chain->applyEffects(s2);
    EXPECT_NE(s2.getPcmValue(), 0.5f);
}

// --- End-to-end mock IO flow ---

TEST_F(DSPTest, MockSamplerReadsSamples)
{
    MockSamplingModule mockSampler(ASSET_PATH + "/input_440.wav");
    float s4 = 0.0f;
    EXPECT_TRUE(mockSampler.getSample(s4));
}

TEST_F(DSPTest, MockOutputWritesSamples)
{
    MockOutputModule mockOutput(ASSET_PATH + "/output_test.wav");
    Sample s5(0.42f);
    EXPECT_NO_THROW(mockOutput.writeSample(s5));
    EXPECT_NO_THROW(mockOutput.saveToFile());
}

TEST_F(DSPTest, FullMockIOPipelineRunsSuccessfully)
{
    MockSamplingModule mockSampler(ASSET_PATH + "/input_440.wav");
    MockOutputModule mockOutput(ASSET_PATH + "/output_test.wav");

    float pcm;
    while (mockSampler.getSample(pcm))
    {
        Sample s6(pcm);
        chain->applyEffects(s6);
        mockOutput.writeSample(s6);
    }

    EXPECT_NO_THROW(mockOutput.saveToFile());
}

// --- Config system behaviour ---

TEST_F(DSPTest, WirePreservesSignalWhenAllEffectsDisabled)
{
    config->set("gain", false, 100.0f);
    config->set("fuzz", false, 100.0f);

    chain->configureEffects(*config);
    Sample s7(0.5f);
    std::cout << "[unit_tests.cpp] Value: ";
    std::cout << std::to_string(s7.getPcmValue());
    std::cout << "\n";
    chain->applyEffects(s7);

    EXPECT_FLOAT_EQ(s7.getPcmValue(), 0.5f);
}

TEST_F(DSPTest, Gain100PercentPreservesAmplitude)
{
    config->set("gain", true, 100.0f);

    chain->configureEffects(*config);

    Sample s(1.0f);
    chain->applyEffects(s);
    EXPECT_NEAR(s.getPcmValue(), 1.0f, 0.01f);
}

TEST_F(DSPTest, FuzzClampsAmplitudeOverThreshold)
{
    config->set("gain", false, 100.0f);
    config->set("fuzz", true, 1.0f); // 0.01 threshold

    chain->configureEffects(*config);

    Sample s(1.0f);
    chain->applyEffects(s);
    EXPECT_NEAR(s.getPcmValue(), 0.01f, 0.001f);
}

TEST_F(DSPTest, Gain200PercentDoublesSample)
{
    config->set("gain", true, 200.0f);
    config->set("fuzz", false, 1.0f);

    chain->configureEffects(*config);

    Sample s(0.5);
    chain->applyEffects(s);
    EXPECT_NEAR(s.getPcmValue(), 1.0f, 0.01f);
}

// --- Entry point ---
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
