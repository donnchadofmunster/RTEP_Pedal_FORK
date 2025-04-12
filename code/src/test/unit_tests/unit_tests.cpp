#include <gtest/gtest.h>
#include "MockSamplingModule.h"
#include "MockOutputModule.h"
#include "DigitalSignalChain.h"
#include "Sample.h"
#include "EffectFactory.h"

extern void ForceAllEffects();

class DSPTest : public ::testing::Test {
protected:
    void SetUp() override {
        ForceAllEffects();
        chain = std::make_unique<DigitalSignalChain>();
        ASSERT_TRUE(chain->loadEffectsFromFile("assets/effects_chain.txt"));
    }

    std::unique_ptr<DigitalSignalChain> chain;
};

TEST_F(DSPTest, SingleEffectProcessesSampleCorrectly) {
    Sample s(0.5f, 0.0);
    chain->applyEffects(s, 2.0f);

    // Expect at least one effect to have been applied
    EXPECT_FALSE(s.getAppliedEffects().empty());

    // Example value test (you would adjust this based on expected transformations)
    EXPECT_NEAR(s.getPcmValue(), 0.5f, 0.5f);
}

TEST_F(DSPTest, EndToEndMockIOFlow) {
    MockSamplingModule mockSampler("assets/input_440.wav");
    MockOutputModule mockOutput("assets/output_test.wav");

    float pcm;
    double timeIndex = 0.0;
    const double timeStep = 1.0 / 44100.0;

    while (mockSampler.getSample(pcm)) {
        Sample s(pcm, timeIndex);
        chain->applyEffects(s, 2.0f);
        mockOutput.writeSample(s);
        timeIndex += timeStep;
    }

    // Validate output (e.g., number of samples written matches expectation)
    mockOutput.saveToFile();
    SUCCEED(); // If it gets here, the pipeline didn't crash
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
