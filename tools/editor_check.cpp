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
    if (editor->getWidth() != 540 || editor->getHeight() != 1000)
        return 1;
    for (const auto &spec : strip::parameterSpecs) {
        auto *control = editor->findChildWithID(spec.id);
        if (control == nullptr || !control->isVisible() || control->getBounds().isEmpty() ||
            !editor->getLocalBounds().contains(control->getBounds())) {
            std::fprintf(stderr, "Missing or clipped control: %s\n", spec.id);
            return 1;
        }
        for (auto *other : editor->getChildren())
            if (control != other && control->getBounds().intersects(other->getBounds())) {
                std::fprintf(stderr, "Overlapping control: %s\n", spec.id);
                return 1;
            }
    }
    auto saveSnapshot = [&](const juce::Image &image, int argument) {
        if (!image.isValid())
            return false;
        if (argc <= argument)
            return true;
        juce::File file = juce::File::getCurrentWorkingDirectory().getChildFile(argv[argument]);
        file.deleteFile();
        juce::FileOutputStream output(file);
        return output.openedOk() && juce::PNGImageFormat().writeImageToStream(image, output);
    };
    auto dark = editor->createComponentSnapshot(editor->getLocalBounds());
    if (!saveSnapshot(dark, 1))
        return 1;
    auto *theme = dynamic_cast<juce::ToggleButton *>(editor->findChildWithID("theme"));
    if (theme == nullptr || !theme->getToggleState())
        return 1;
    theme->setToggleState(false, juce::sendNotification);
    auto light = editor->createComponentSnapshot(editor->getLocalBounds());
    if (!saveSnapshot(light, 2) ||
        dark.getPixelAt(0, 0).getBrightness() >= light.getPixelAt(0, 0).getBrightness())
        return 1;
    theme->setToggleState(true, juce::sendNotification);
    auto restored = editor->createComponentSnapshot(editor->getLocalBounds());
    if (restored.getPixelAt(0, 0) != dark.getPixelAt(0, 0))
        return 1;
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
    std::puts("PASS: state restoration, control layout, dark/light snapshots, oversized host buffer");
    return 0;
}
