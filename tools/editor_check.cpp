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
    if (theme == nullptr || !theme->getToggleState() || dark.getPixelAt(0, 0) != juce::Colour(0xff161918))
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
    auto *palette = dynamic_cast<juce::ComboBox *>(editor->findChildWithID("palette"));
    if (palette == nullptr || palette->getNumItems() != 6 ||
        !editor->getLocalBounds().contains(palette->getBounds()) ||
        palette->getWidth() != palette->getHeight() || palette->getX() <= theme->getRight() ||
        palette->getY() != theme->getY())
        return 1;
    for (auto *other : editor->getChildren())
        if (other != palette && palette->getBounds().intersects(other->getBounds()))
            return 1;
    const auto paletteIcon = palette->createComponentSnapshot(palette->getLocalBounds());
    for (int y = 7; y <= 19; ++y) {
        const bool line = (y - 7) % 5 < 2;
        const auto expected = editor->getLookAndFeel().findColour(line ? juce::ComboBox::textColourId
                                                                       : juce::ComboBox::backgroundColourId);
        if (paletteIcon.getPixelAt(12, y) != expected)
            return 1;
    }
    struct ThemeCheck {
        juce::uint32 background, primary, onPrimary, accent, onAccent;
    };
    const ThemeCheck expected[][2]{{{0xfff7f9fc, 0xff335cff, 0xffffffff, 0xff13b8a6, 0xff052e2b},
                                    {0xff0d1321, 0xff8aa2ff, 0xff0b1844, 0xff45dbc9, 0xff073a35}},
                                   {{0xfff4f8fa, 0xff087d75, 0xffffffff, 0xff7054cc, 0xffffffff},
                                    {0xff0b1220, 0xff68e0cf, 0xff072725, 0xffa78bfa, 0xff21153b}},
                                   {{0xfff8f3ea, 0xffb6533d, 0xffffffff, 0xff78926a, 0xff102214},
                                    {0xff1b1917, 0xffe49a82, 0xff34201a, 0xffb1c39e, 0xff1f3021}},
                                   {{0xfff6f4ff, 0xff7357eb, 0xffffffff, 0xffe5a9c6, 0xff45283b},
                                    {0xff151225, 0xffb5a4ff, 0xff241a4d, 0xfff0b9d7, 0xff44243b}},
                                   {{0xfff7f8f4, 0xff496d00, 0xffffffff, 0xff087a7c, 0xffffffff},
                                    {0xff111417, 0xffd9ff57, 0xff243000, 0xff61c0bf, 0xff092f32}}};
    for (int i = 0; i < 5; ++i) {
        palette->setSelectedId(i + 2, juce::sendNotificationSync);
        for (int mode = 0; mode < 2; ++mode) {
            theme->setToggleState(mode != 0, juce::sendNotification);
            const auto image = editor->createComponentSnapshot(editor->getLocalBounds());
            const auto &colours = expected[i][mode];
            auto &look = editor->getLookAndFeel();
            bool foundOnPrimary = false;
            for (int y = theme->getY(); y < theme->getBottom(); ++y)
                for (int x = theme->getX(); x < theme->getRight(); ++x)
                    foundOnPrimary |= image.getPixelAt(x, y) == juce::Colour(colours.onPrimary);
            if (image.getPixelAt(0, 0) != juce::Colour(colours.background) || (mode && !foundOnPrimary) ||
                (mode &&
                 image.getPixelAt(theme->getX() + 2, theme->getY() + 2) != juce::Colour(colours.primary)) ||
                look.findColour(juce::PopupMenu::highlightedBackgroundColourId) !=
                    juce::Colour(colours.accent) ||
                look.findColour(juce::PopupMenu::highlightedTextColourId) != juce::Colour(colours.onAccent)) {
                std::fprintf(stderr, "Theme mismatch: %s (%s)\n", palette->getText().toRawUTF8(),
                             mode ? "dark" : "light");
                return 1;
            }
        }
    }
    palette->setSelectedId(1, juce::sendNotificationSync);
    if (editor->createComponentSnapshot(editor->getLocalBounds()).getPixelAt(0, 0) != dark.getPixelAt(0, 0))
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
    std::puts("PASS: state restoration, control layout, all six dark/light palettes, oversized host buffer");
    return 0;
}
