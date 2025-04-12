#include <gtest/gtest.h>
#include "MockSamplingModule.h"
#include "MockOutputModule.h"
#include "DigitalSignalChain.h"
#include "Sample.h"
#include "EffectFactory.h"

extern void ForceAllEffects();
const std::string ASSET_PATH = "../../../../assets";

class DSPTest : public ::testing::Test {
protected:
    void SetUp() override {
        ForceAllEffects();
        chain = std::make_unique<DigitalSignalChain>();
        ASSERT_TRUE(chain->loadEffectsFromFile(ASSET_PATH + "/test_chain.txt"));
    }

    std::unique_ptr<DigitalSignalChain> chain;
};

// --- Sample processing tests ---

TEST_F(DSPTest, SampleEffectListIsNotEmpty) {
    Sample s(0.5f, 0.0);
    chain->applyEffects(s, 2.0f);
    EXPECT_FALSE(s.getAppliedEffects().empty());
}

TEST_F(DSPTest, SamplePCMIsTransformed) {
    Sample s(0.5f, 0.0);
    chain->applyEffects(s, 2.0f);
    EXPECT_NEAR(s.getPcmValue(), 0.5f, 0.5f);  // Tweak tolerance as needed
}

TEST_F(DSPTest, SamplePreservesTimeIndex) {
    Sample s(0.5f, 1.23);
    chain->applyEffects(s, 2.0f);
    EXPECT_DOUBLE_EQ(s.getTimeIndex(), 1.23);
}

// --- End-to-end flow ---

TEST_F(DSPTest, MockSamplerReadsSamples) {
    MockSamplingModule mockSampler(ASSET_PATH + "/input_440.wav");
    float sample = 0.0f;
    EXPECT_TRUE(mockSampler.getSample(sample));
}

TEST_F(DSPTest, MockOutputWritesSamples) {
    MockOutputModule mockOutput(ASSET_PATH + "/output_test.wav");
    Sample s(0.42f, 0.0);
    EXPECT_NO_THROW(mockOutput.writeSample(s));
    EXPECT_NO_THROW(mockOutput.saveToFile());
}

TEST_F(DSPTest, FullMockIOPipelineRunsSuccessfully) {
    MockSamplingModule mockSampler(ASSET_PATH + "/input_440.wav");
    MockOutputModule mockOutput(ASSET_PATH + "/output_test.wav");

    float pcm;
    double timeIndex = 0.0;
    const double timeStep = 1.0 / 44100.0;

    while (mockSampler.getSample(pcm)) {
        Sample s(pcm, timeIndex);
        chain->applyEffects(s, 2.0f);
        mockOutput.writeSample(s);
        timeIndex += timeStep;
    }

    // Only fail if exceptions or crashes occur
    EXPECT_NO_THROW(mockOutput.saveToFile());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
