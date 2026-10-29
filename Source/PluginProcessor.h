#pragma once
#include "Engine.h"
#include "Parameters.h"
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>

class StripAudioProcessor final : public juce::AudioProcessor,
                                  private juce::AudioProcessorValueTreeState::Listener,
                                  private juce::AsyncUpdater {
    std::atomic<int> activeBank{0};

  public:
    StripAudioProcessor();
    ~StripAudioProcessor() override;
    void prepareToPlay(double sampleRate, int blockSize) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout &) const override;
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    void processBlockBypassed(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override {
        return true;
    }
    const juce::String getName() const override {
        return JucePlugin_Name;
    }
    bool acceptsMidi() const override {
        return false;
    }
    bool producesMidi() const override {
        return false;
    }
    double getTailLengthSeconds() const override {
        return 0.0;
    }
    int getNumPrograms() override {
        return 4;
    }
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int i) override {
        return juce::String::charToString(static_cast<juce::juce_wchar>('A' + juce::jlimit(0, 3, i)));
    }
    void changeProgramName(int, const juce::String &) override {}
    void getStateInformation(juce::MemoryBlock &) override;
    void setStateInformation(const void *, int) override;
    juce::AudioProcessorParameter *getBypassParameter() const override;
    juce::AudioProcessorValueTreeState state;
    struct MeterValues {
        float inputL, inputR, outputL, outputR, compression, gate;
    };
    MeterValues meterValues() const noexcept;

  private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout(std::atomic<int> &);
    void render(juce::AudioBuffer<float> &, bool hostBypass);
    void parameterChanged(const juce::String &, float) override;
    void handleAsyncUpdate() override;
    juce::CriticalSection bankLock;
    std::atomic<bool> restoring{false};
    strip::Engine engine;
    std::array<juce::RangedAudioParameter *, strip::parameterCount> parameters{};
    std::array<std::atomic<float>, 6> meterAtoms{};
    std::vector<float> monoScratch;
};
