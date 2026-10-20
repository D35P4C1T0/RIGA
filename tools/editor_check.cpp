#include "PluginProcessor.h"
#include <cstdio>

int main(int argc, char **argv) {
    juce::ScopedJuceInitialiser_GUI gui;
    StripAudioProcessor processor;
    processor.prepareToPlay(48000, 256);
    auto *gain = processor.state.getParameter("inputGain");
    gain->setValueNotifyingHost(.731f);
    processor.setCurrentProgram(1);
    if (std::abs(gain->getValue() - .5f) > 1e-6f)
        return 1;
    processor.setCurrentProgram(0);
    if (std::abs(gain->getValue() - .731f) > 1e-6f)
        return 1;
    juce::MemoryBlock saved;
    processor.getStateInformation(saved);
    gain->setValueNotifyingHost(.2f);
    processor.setStateInformation(saved.getData(), static_cast<int>(saved.getSize()));
    if (std::abs(gain->getValue() - .731f) > 1e-6f) {
        std::fputs("State restoration failed\n", stderr);
        return 1;
    }
    gain->setValueNotifyingHost(.5f);
    std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());
    auto image = editor->createComponentSnapshot(editor->getLocalBounds());
    if (!image.isValid())
        return 1;
    if (argc > 1) {
        juce::File file = juce::File::getCurrentWorkingDirectory().getChildFile(argv[1]);
        file.deleteFile();
        juce::FileOutputStream output(file);
        if (!output.openedOk() || !juce::PNGImageFormat().writeImageToStream(image, output))
            return 1;
    }
    juce::AudioBuffer<float> audio(2, 1024);
    audio.clear();
    audio.setSample(0, 0, .5f);
    juce::MidiBuffer midi;
    processor.processBlock(audio, midi);
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < audio.getNumSamples(); ++i)
            if (!std::isfinite(audio.getSample(ch, i)))
                return 1;
    StripAudioProcessor mono;
    auto layout = mono.getBusesLayout();
    layout.inputBuses.set(0, juce::AudioChannelSet::mono());
    layout.outputBuses.set(0, juce::AudioChannelSet::mono());
    if (!mono.setBusesLayout(layout))
        return 1;
    mono.prepareToPlay(48000, 256);
    juce::AudioBuffer<float> monoAudio(1, 1024);
    monoAudio.clear();
    monoAudio.setSample(0, 0, .5f);
    mono.processBlock(monoAudio, midi);
    for (int i = 0; i < monoAudio.getNumSamples(); ++i)
        if (!std::isfinite(monoAudio.getSample(0, i)))
            return 1;
    std::puts("PASS: state restoration, editor snapshot, oversized host buffer");
    return 0;
}
