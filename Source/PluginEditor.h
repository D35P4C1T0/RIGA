#pragma once
#include "PluginProcessor.h"
#include <juce_audio_utils/juce_audio_utils.h>

class StripAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer {
  public:
    explicit StripAudioProcessorEditor(StripAudioProcessor &);
    ~StripAudioProcessorEditor() override;
    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    class Style;
    class Control;
    void timerCallback() override;
    void place(strip::Param, int x, int y, int width, int height);
    StripAudioProcessor &processor;
    std::unique_ptr<Style> style;
    std::array<std::unique_ptr<Control>, strip::parameterCount> controls;
    juce::TooltipWindow tooltips{this, 600};
    StripAudioProcessor::MeterValues levels{};
};
