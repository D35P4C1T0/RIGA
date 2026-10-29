#include "PluginEditor.h"
#include <cmath>

namespace {
struct Palette {
    juce::Colour background{0xffeeede7}, panel{0xfffaf9f5}, ink{0xff202320}, muted{0xff70736c};
    juce::Colour red{0xffdf3c2f}, rule{0xffd4d5cc}, track{0xffdedfd6};
    juce::Colour onPrimary{0xffffffff}, secondary{0xffdf3c2f}, onSecondary{0xffffffff};
};
struct Theme {
    const char *name;
    Palette light, dark;
};
const Theme themes[]{
    {"Arctic Blue",
     {juce::Colour(0xfff7f9fc), juce::Colour(0xffffffff), juce::Colour(0xff121826), juce::Colour(0xff58677b),
      juce::Colour(0xff335cff), juce::Colour(0xffe2e8f0), juce::Colour(0xffe2e8f0), juce::Colour(0xffffffff),
      juce::Colour(0xff13b8a6), juce::Colour(0xff052e2b)},
     {juce::Colour(0xff0d1321), juce::Colour(0xff182338), juce::Colour(0xfff3f7ff), juce::Colour(0xffaab6c9),
      juce::Colour(0xff8aa2ff), juce::Colour(0xff303e55), juce::Colour(0xff303e55), juce::Colour(0xff0b1844),
      juce::Colour(0xff45dbc9), juce::Colour(0xff073a35)}},
    {"Midnight Mint",
     {juce::Colour(0xfff4f8fa), juce::Colour(0xffffffff), juce::Colour(0xff14212d), juce::Colour(0xff566c79),
      juce::Colour(0xff087d75), juce::Colour(0xffdce6eb), juce::Colour(0xffdce6eb), juce::Colour(0xffffffff),
      juce::Colour(0xff7054cc), juce::Colour(0xffffffff)},
     {juce::Colour(0xff0b1220), juce::Colour(0xff161f30), juce::Colour(0xfff3f7fb), juce::Colour(0xffa7b5c7),
      juce::Colour(0xff68e0cf), juce::Colour(0xff303d52), juce::Colour(0xff303d52), juce::Colour(0xff072725),
      juce::Colour(0xffa78bfa), juce::Colour(0xff21153b)}},
    {"Warm Editorial",
     {juce::Colour(0xfff8f3ea), juce::Colour(0xfffffcf7), juce::Colour(0xff322c2a), juce::Colour(0xff70645e),
      juce::Colour(0xffb6533d), juce::Colour(0xffe4d9ce), juce::Colour(0xffe4d9ce), juce::Colour(0xffffffff),
      juce::Colour(0xff78926a), juce::Colour(0xff102214)},
     {juce::Colour(0xff1b1917), juce::Colour(0xff292522), juce::Colour(0xfff7f0e8), juce::Colour(0xffc1b2a5),
      juce::Colour(0xffe49a82), juce::Colour(0xff443b34), juce::Colour(0xff443b34), juce::Colour(0xff34201a),
      juce::Colour(0xffb1c39e), juce::Colour(0xff1f3021)}},
    {"Digital Lavender",
     {juce::Colour(0xfff6f4ff), juce::Colour(0xffffffff), juce::Colour(0xff24213a), juce::Colour(0xff69627d),
      juce::Colour(0xff7357eb), juce::Colour(0xffe5e0f5), juce::Colour(0xffe5e0f5), juce::Colour(0xffffffff),
      juce::Colour(0xffe5a9c6), juce::Colour(0xff45283b)},
     {juce::Colour(0xff151225), juce::Colour(0xff231d38), juce::Colour(0xfff6f3ff), juce::Colour(0xffb5abc9),
      juce::Colour(0xffb5a4ff), juce::Colour(0xff403653), juce::Colour(0xff403653), juce::Colour(0xff241a4d),
      juce::Colour(0xfff0b9d7), juce::Colour(0xff44243b)}},
    {"Graphite Lime",
     {juce::Colour(0xfff7f8f4), juce::Colour(0xffffffff), juce::Colour(0xff202725), juce::Colour(0xff5d6b65),
      juce::Colour(0xff496d00), juce::Colour(0xffdde3db), juce::Colour(0xffdde3db), juce::Colour(0xffffffff),
      juce::Colour(0xff087a7c), juce::Colour(0xffffffff)},
     {juce::Colour(0xff111417), juce::Colour(0xff23282a), juce::Colour(0xfff4f3ef), juce::Colour(0xffb5bfba),
      juce::Colour(0xffd9ff57), juce::Colour(0xff3b4544), juce::Colour(0xff3b4544), juce::Colour(0xff243000),
      juce::Colour(0xff61c0bf), juce::Colour(0xff092f32)}},
};
juce::Font type(float size, bool bold = false) {
    return juce::Font(juce::FontOptions(
#if JUCE_MAC
        "Helvetica Neue",
#else
        "Arial",
#endif
        size, bold ? juce::Font::bold : juce::Font::plain));
}
juce::Colour accent(strip::Param parameter, bool curveB = false, bool extendedRange = false) {
    using P = strip::Param;
    switch (parameter) {
    case P::highGain:
    case P::highFreq:
    case P::noise:
        return juce::Colour(0xffd23431);
    case P::highMidGain:
    case P::highMidFreq:
    case P::highMidQ:
        return juce::Colour(0xff4d7e47);
    case P::gateRange:
    case P::gateThreshold:
    case P::gateRelease:
        return juce::Colour(extendedRange ? 0xff2fa185 : 0xff4d7e47);
    case P::lowMidGain:
    case P::lowMidFreq:
    case P::lowMidQ:
        return juce::Colour(0xff4381b2);
    case P::lowGain:
    case P::lowFreq:
        return juce::Colour(curveB ? 0xff765142 : 0xff3b3b3b);
    case P::compMix:
    case P::sidechainHP:
    case P::secondThreshold:
    case P::hysteresis:
    case P::drive:
        return juce::Colour(0xff9b7c43);
    default:
        return juce::Colour(0xffe6e6e6);
    }
}
juce::String shortName(strip::Param p) {
    using P = strip::Param;
    switch (p) {
    case P::hpfFreq:
    case P::lpfFreq:
    case P::highFreq:
    case P::highMidFreq:
    case P::lowMidFreq:
    case P::lowFreq:
        return "FREQUENCY";
    case P::highGain:
    case P::highMidGain:
    case P::lowMidGain:
    case P::lowGain:
        return "GAIN";
    case P::highMidQ:
    case P::lowMidQ:
        return "Q";
    case P::ratio:
        return "RATIO";
    case P::compThreshold:
    case P::gateThreshold:
        return "THRESHOLD";
    case P::compRelease:
    case P::gateRelease:
        return "RELEASE";
    case P::compFast:
    case P::gateFast:
        return "FAST ATTACK";
    case P::hpfOn:
        return "HP";
    case P::lpfOn:
        return "LP";
    case P::hpfTriple:
        return "HP x3";
    case P::lpfThird:
        return "LP /3";
    case P::filterPosition:
        return "FILTER ROUTE";
    case P::compMix:
        return "MIX";
    case P::compLink:
        return "LINK";
    case P::sidechainHP:
        return "KEY HIGH PASS";
    case P::secondThreshold:
        return "2ND THRESHOLD";
    case P::gateRange:
        return "RANGE";
    case P::gateMode:
        return "GATE MODE";
    case P::gateInvert:
        return "INVERT";
    case P::gateThresholdRange:
        return "THRESH RANGE";
    case P::hysteresis:
        return "HYSTERESIS";
    case P::vcaMode:
        return "VCA SERIES";
    case P::externalKey:
        return "EXTERNAL KEY";
    case P::dynamicsOn:
        return "DYNAMICS";
    case P::highBell:
    case P::lowBell:
        return "BELL";
    case P::eqType:
        return "EQ CURVE";
    case P::eqOn:
        return "EQ";
    case P::eqPosition:
        return "EQ ROUTING";
    case P::meterPosition:
        return "METER SOURCE";
    case P::stereoMode:
        return "VARIATION";
    case P::inputGain:
        return "INPUT";
    case P::outputGain:
        return "OUTPUT";
    case P::noise:
        return "NOISE";
    default:
        return juce::String(strip::parameterSpecs[strip::index(p)].name).toUpperCase();
    }
}
} // namespace

class StripAudioProcessorEditor::Style final : public juce::LookAndFeel_V4, public Palette {
  public:
    Style() {
        setTheme(0, true);
    }
    void setTheme(int selected, bool dark) {
        static_cast<Palette &>(*this) =
            dark ? Palette{juce::Colour(0xff161918), juce::Colour(0xff202422), juce::Colour(0xffeeede7),
                           juce::Colour(0xffa6ada6), juce::Colour(0xfff05a47), juce::Colour(0xff414843),
                           juce::Colour(0xff353d37), juce::Colours::white,     juce::Colour(0xfff05a47),
                           juce::Colours::white}
                 : Palette{};
        if (selected > 0)
            static_cast<Palette &>(*this) = dark ? themes[selected - 1].dark : themes[selected - 1].light;
        setColour(juce::Label::textColourId, ink);
        setColour(juce::ComboBox::backgroundColourId, panel);
        setColour(juce::ComboBox::outlineColourId, rule);
        setColour(juce::ComboBox::textColourId, ink);
        setColour(juce::PopupMenu::backgroundColourId, panel);
        setColour(juce::PopupMenu::textColourId, ink);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, secondary);
        setColour(juce::PopupMenu::highlightedTextColourId, onSecondary);
        setColour(juce::TooltipWindow::backgroundColourId, ink);
        setColour(juce::TooltipWindow::textColourId, panel);
        setColour(juce::TooltipWindow::outlineColourId, ink);
        setColour(juce::TextEditor::textColourId, ink);
        setColour(juce::TextEditor::backgroundColourId, panel);
        setColour(juce::TextEditor::highlightColourId, red.withAlpha(.2f));
        setColour(juce::Slider::textBoxTextColourId, ink);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    }
    juce::Font getLabelFont(juce::Label &label) override {
        return label.getFont();
    }
    juce::Font getComboBoxFont(juce::ComboBox &) override {
        return type(11.5f);
    }
    juce::Font getPopupMenuFont() override {
        return type(12);
    }
    juce::Label *createSliderTextBox(juce::Slider &slider) override {
        auto *label = LookAndFeel_V4::createSliderTextBox(slider);
        label->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        label->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        label->setFont(type(11.5f, true));
        label->setColour(juce::TextEditor::highlightColourId, red.withAlpha(.2f));
        return label;
    }
    void drawLabel(juce::Graphics &g, juce::Label &label) override {
        if (label.isBeingEdited())
            return;
        const bool value = dynamic_cast<juce::Slider *>(label.getParentComponent()) != nullptr;
        g.setColour(ink);
        g.setFont(value ? type(11.5f, true) : label.getFont());
        g.drawFittedText(label.getText(), label.getLocalBounds().reduced(2, 0), label.getJustificationType(),
                         1);
        if (value) {
            g.setColour(label.isMouseOver() ? red : rule);
            g.drawHorizontalLine(label.getHeight() - 1, 6, float(label.getWidth() - 6));
        }
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int w, int h, float position, float start,
                          float end, juce::Slider &slider) override {
        const float radius = juce::jmin(22.0f, juce::jmin(float(w), float(h)) * .5f - 4);
        if (radius <= 0)
            return;
        const juce::Point<float> c(float(x) + float(w) * .5f, float(y) + float(h) * .5f);
        const float angle = start + position * (end - start);
        juce::Path base, value;
        base.addCentredArc(c.x, c.y, radius, radius, 0, start, end, true);
        const bool bipolar = slider.getProperties().getWithDefault("bipolar", false);
        const float origin = bipolar ? (start + end) * .5f : start;
        value.addCentredArc(c.x, c.y, radius, radius, 0, juce::jmin(origin, angle), juce::jmax(origin, angle),
                            true);
        g.setColour(track);
        g.strokePath(base, juce::PathStrokeType(2));
        const auto colour = slider.findColour(juce::Slider::rotarySliderFillColourId);
        g.setColour(colour);
        g.strokePath(value, juce::PathStrokeType(2));
        g.fillEllipse(c.x - radius + 5, c.y - radius + 5, (radius - 5) * 2, (radius - 5) * 2);
        g.setColour(ink.withAlpha(.35f));
        g.drawEllipse(c.x - radius + 5, c.y - radius + 5, (radius - 5) * 2, (radius - 5) * 2, .8f);
        g.setColour(colour.getPerceivedBrightness() > .65f ? juce::Colour(0xff202320)
                                                           : juce::Colour(0xfffaf9f5));
        g.drawLine(c.x + std::sin(angle) * 1, c.y - std::cos(angle) * 1, c.x + std::sin(angle) * (radius - 8),
                   c.y - std::cos(angle) * (radius - 8), 2);
        if (bipolar) {
            g.setColour(muted);
            g.fillRect(c.x - .5f, c.y - radius - 3, 1.0f, 3.0f);
        }
    }
    void drawToggleButton(juce::Graphics &g, juce::ToggleButton &button, bool hover, bool down) override {
        auto r = button.getLocalBounds().toFloat().reduced(1);
        const bool active = button.getToggleState();
        g.setColour(active ? red : down ? track : hover ? background : panel);
        g.fillRect(r);
        if (!active) {
            g.setColour(hover ? muted : rule);
            g.drawRect(r, .7f);
        }
        g.setColour(active ? onPrimary : ink);
        g.setFont(type(9.5f, true));
        g.drawFittedText(button.getButtonText(), r.toNearestInt().reduced(4, 1), juce::Justification::centred,
                         2);
    }
    void drawComboBox(juce::Graphics &g, int w, int h, bool, int, int, int, int,
                      juce::ComboBox &box) override {
        auto r = juce::Rectangle<float>(.5f, .5f, float(w) - 1, float(h) - 1);
        g.setColour(panel);
        g.fillRect(r);
        g.setColour(rule);
        g.drawRect(r, .7f);
        if (box.getComponentID() == "palette") {
            g.setColour(box.hasKeyboardFocus(true) ? red : ink);
            for (int i = 0; i < 3; ++i)
                g.fillRect(6, h / 2 - 6 + i * 5, w - 12, 2);
            return;
        }
        juce::Path arrow;
        arrow.startNewSubPath(float(w) - 15, float(h) * .45f);
        arrow.lineTo(float(w) - 11, float(h) * .6f);
        arrow.lineTo(float(w) - 7, float(h) * .45f);
        g.setColour(ink);
        g.strokePath(arrow, juce::PathStrokeType(1));
    }
    void positionComboBoxText(juce::ComboBox &box, juce::Label &label) override {
        if (box.getComponentID() == "palette") {
            label.setBounds(0, 0, 0, 0);
            return;
        }
        label.setBounds(7, 1, box.getWidth() - 25, box.getHeight() - 2);
        label.setFont(type(11.5f));
    }
    void drawLinearSlider(juce::Graphics &g, int x, int y, int w, int h, float position, float, float,
                          juce::Slider::SliderStyle, juce::Slider &) override {
        const float middle = float(x) + float(w) * .5f;
        g.setColour(track);
        g.fillRect(middle - 1, float(y) + 4, 2.0f, float(h) - 8);
        g.setColour(red);
        g.fillRect(middle - 1, position, 2.0f, float(y + h) - 4 - position);
        g.setColour(ink);
        g.fillRect(middle - 13, position - 4, 26.0f, 8.0f);
        g.setColour(panel);
        g.drawHorizontalLine(int(position), middle - 8, middle + 8);
    }
};

class StripAudioProcessorEditor::Control final : public juce::Component {
  public:
    Control(StripAudioProcessor &p, strip::Param parameter) : processor(p), id(parameter) {
        const auto &spec = strip::parameterSpecs[strip::index(id)];
        setComponentID(spec.id);
        name.setText(shortName(id), juce::dontSendNotification);
        name.setFont(type(9.5f, true));
        name.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(name);
        if (spec.steps == 1) {
            toggle.setButtonText(shortName(id));
            toggle.setTooltip(spec.name);
            toggle.setColour(juce::ToggleButton::tickColourId, accent(id));
            toggle.onStateChange = [this] {
                const auto text = juce::String(strip::parameterText(id, toggle.getToggleState() ? 1.0 : 0.0));
                juce::String label = shortName(id);
                using P = strip::Param;
                if (id == P::highBell || id == P::lowBell)
                    label = text.toUpperCase();
                else if (id == P::gateMode)
                    label = text.toUpperCase();
                else if (id == P::gateThresholdRange)
                    label = "RANGE " + text;
                else if (id == P::vcaMode)
                    label = toggle.getToggleState() ? "VCA B" : "VCA A";
                else if (id == P::eqType)
                    label = toggle.getToggleState() ? "CURVE B" : "CURVE A";
                else if (id == P::stereoMode)
                    label = text.toUpperCase();
                else if (id == P::filterPosition)
                    label = "ROUTE / " + text.toUpperCase();
                else if (id == P::meterPosition)
                    label = "METER / " + text.toUpperCase();
                toggle.setButtonText(label);
            };
            addAndMakeVisible(toggle);
            buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                p.state, spec.id, toggle);
            toggle.onStateChange();
            name.setVisible(false);
        } else if (spec.steps > 1) {
            for (int i = 0; i <= spec.steps; ++i)
                choices.addItem(id == strip::Param::eqPosition
                                    ? juce::String(i == 0   ? "Pre"
                                                   : i == 1 ? "Key"
                                                            : "Post")
                                    : juce::String(strip::parameterText(id, double(i) / spec.steps)),
                                i + 1);
            choices.setTooltip(spec.name);
            addAndMakeVisible(choices);
            comboAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                p.state, spec.id, choices);
        } else {
            slider.setSliderStyle(id == strip::Param::outputGain
                                      ? juce::Slider::LinearVertical
                                      : juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 18);
            slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                                       juce::MathConstants<float>::pi * 2.75f, true);
            slider.getProperties().set("bipolar",
                                       id == strip::Param::highGain || id == strip::Param::highMidGain ||
                                           id == strip::Param::lowMidGain || id == strip::Param::lowGain ||
                                           id == strip::Param::inputGain);
            refreshColour();
            slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
            slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
            slider.setTooltip(spec.name);
            slider.setDoubleClickReturnValue(true, spec.initial);
            addAndMakeVisible(slider);
            sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                p.state, spec.id, slider);
            if (id == strip::Param::hysteresis) {
                slider.textFromValueFunction = [this](double n) {
                    return n == 0 ? juce::String("0.0 dB") : juce::String(strip::parameterText(id, n));
                };
                slider.updateText();
            }
            if (id == strip::Param::hpfFreq || id == strip::Param::lpfFreq ||
                id == strip::Param::gateThreshold) {
                slider.textFromValueFunction = [this](double n) {
                    double value = strip::physical(id, n);
                    if (id == strip::Param::gateThreshold)
                        return juce::String(value + (context() ? -30 : 0), 1) + " dB";
                    value *= id == strip::Param::hpfFreq ? (context() ? 3 : 1) : (context() ? 1.0 / 3 : 1);
                    return value >= 1000 ? juce::String(value * .001, 1) + " kHz"
                                         : juce::String(value, 1) + " Hz";
                };
                slider.valueFromTextFunction = [this](const juce::String &text) {
                    double target = text.getDoubleValue();
                    if (text.containsIgnoreCase("khz"))
                        target *= 1000;
                    if (id == strip::Param::gateThreshold)
                        target += context() ? 30 : 0;
                    else
                        target /=
                            id == strip::Param::hpfFreq ? (context() ? 3 : 1) : (context() ? 1.0 / 3 : 1);
                    double best = 0, error = 1e30;
                    for (int i = 0; i <= 10000; ++i) {
                        double n = double(i) / 10000, d = std::abs(strip::physical(id, n) - target);
                        if (d < error) {
                            best = n;
                            error = d;
                        }
                    }
                    return best;
                };
                slider.updateText();
            }
        }
    }
    void refreshContext() {
        if (sliderAttachment)
            refreshColour();
        if (id != strip::Param::hpfFreq && id != strip::Param::lpfFreq && id != strip::Param::gateThreshold)
            return;
        const bool next = context();
        if (next != lastContext) {
            lastContext = next;
            slider.updateText();
        }
    }
    void resized() override {
        auto r = getLocalBounds();
        if (buttonAttachment)
            toggle.setBounds(r);
        else {
            name.setBounds(r.removeFromTop(13));
            if (comboAttachment)
                choices.setBounds(r.reduced(1, 1));
            else
                slider.setBounds(r);
        }
    }

  private:
    void refreshColour() {
        const bool curveB = processor.state.getRawParameterValue("eqType")->load() >= .5f;
        const bool extendedRange = processor.state.getRawParameterValue("gateThresholdRange")->load() > .5f;
        const auto colour = accent(id, curveB, extendedRange);
        if (slider.findColour(juce::Slider::rotarySliderFillColourId) != colour)
            slider.setColour(juce::Slider::rotarySliderFillColourId, colour);
    }
    bool context() const {
        const auto key = id == strip::Param::hpfFreq   ? "hpfTriple"
                         : id == strip::Param::lpfFreq ? "lpfThird"
                                                       : "gateThresholdRange";
        return processor.state.getRawParameterValue(key)->load() >= .5f;
    }
    bool lastContext = false;
    StripAudioProcessor &processor;
    strip::Param id;
    juce::Label name;
    juce::Slider slider;
    juce::ToggleButton toggle;
    juce::ComboBox choices;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboAttachment;
};

StripAudioProcessorEditor::StripAudioProcessorEditor(StripAudioProcessor &p)
    : AudioProcessorEditor(p), processor(p), style(std::make_unique<Style>()) {
    setLookAndFeel(style.get());
    for (std::size_t i = 0; i < controls.size(); ++i) {
        controls[i] = std::make_unique<Control>(processor, static_cast<strip::Param>(i));
        addAndMakeVisible(*controls[i]);
    }
    theme.setComponentID("theme");
    theme.setButtonText("DARK");
    theme.setTooltip("Switch between dark and light themes");
    theme.setToggleState(true, juce::dontSendNotification);
    palette.setComponentID("palette");
    palette.setTooltip("Choose a colour palette");
    palette.setTitle("Colour palette");
    palette.addItem("Original", 1);
    for (int i = 0; i < int(std::size(themes)); ++i)
        palette.addItem(themes[i].name, i + 2);
    palette.setSelectedId(1, juce::dontSendNotification);
    palette.onChange = [this] {
        style->setTheme(palette.getSelectedId() - 1, theme.getToggleState());
        sendLookAndFeelChange();
        repaint();
    };
    addAndMakeVisible(palette);
    theme.onClick = [this] {
        style->setTheme(palette.getSelectedId() - 1, theme.getToggleState());
        theme.setButtonText(theme.getToggleState() ? "DARK" : "LIGHT");
        sendLookAndFeelChange();
        repaint();
    };
    addAndMakeVisible(theme);
    setSize(540, 1000);
    startTimerHz(30);
}
StripAudioProcessorEditor::~StripAudioProcessorEditor() {
    stopTimer();
    setLookAndFeel(nullptr);
}
void StripAudioProcessorEditor::place(strip::Param p, int x, int y, int w, int h) {
    controls[strip::index(p)]->setBounds(x, y, w, h);
}
void StripAudioProcessorEditor::resized() {
    using P = strip::Param;
    palette.setBounds(418, 24, 26, 26);
    theme.setBounds(348, 24, 62, 26);
    place(P::setting, 272, 10, 66, 44);
    place(P::bypass, 450, 24, 74, 26);
    place(P::hpfFreq, 28, 104, 100, 76);
    place(P::lpfFreq, 140, 104, 100, 76);
    place(P::hpfOn, 28, 182, 42, 22);
    place(P::hpfTriple, 74, 182, 54, 22);
    place(P::lpfOn, 140, 182, 42, 22);
    place(P::lpfThird, 186, 182, 54, 22);
    place(P::filterPosition, 28, 210, 212, 22);
    const P comp[]{P::ratio,   P::compThreshold, P::compRelease,
                   P::compMix, P::sidechainHP,   P::secondThreshold};
    for (int i = 0; i < 6; ++i)
        place(comp[i], 288 + (i % 3) * 72, 104 + (i / 3) * 70, 72, 70);
    place(P::compFast, 288, 248, 104, 22);
    place(P::compLink, 396, 248, 116, 22);
    place(P::gateRange, 288, 320, 72, 72);
    place(P::gateThreshold, 360, 320, 72, 72);
    place(P::gateRelease, 432, 320, 72, 72);
    place(P::hysteresis, 288, 396, 72, 72);
    place(P::gateFast, 366, 400, 146, 24);
    place(P::gateMode, 366, 430, 88, 24);
    place(P::gateInvert, 458, 430, 54, 24);
    place(P::gateThresholdRange, 288, 470, 108, 22);
    place(P::vcaMode, 400, 470, 112, 22);
    place(P::externalKey, 288, 498, 108, 22);
    place(P::dynamicsOn, 400, 498, 112, 22);
    place(P::highGain, 28, 286, 100, 80);
    place(P::highFreq, 140, 286, 100, 80);
    place(P::highBell, 154, 368, 86, 22);
    place(P::highMidGain, 28, 444, 72, 96);
    place(P::highMidFreq, 100, 444, 72, 96);
    place(P::highMidQ, 172, 444, 72, 96);
    place(P::lowMidGain, 28, 600, 72, 96);
    place(P::lowMidFreq, 100, 600, 72, 96);
    place(P::lowMidQ, 172, 600, 72, 96);
    place(P::lowGain, 28, 754, 100, 80);
    place(P::lowFreq, 140, 754, 100, 80);
    place(P::lowBell, 154, 836, 86, 22);
    place(P::eqType, 28, 920, 84, 28);
    place(P::eqOn, 118, 920, 44, 28);
    place(P::eqPosition, 170, 906, 78, 42);
    place(P::inputGain, 288, 570, 72, 76);
    place(P::noise, 360, 570, 72, 76);
    place(P::drive, 432, 570, 72, 76);
    place(P::channel, 288, 650, 108, 40);
    place(P::monitor, 404, 650, 108, 40);
    place(P::stereoMode, 288, 698, 108, 24);
    place(P::power, 400, 698, 54, 24);
    place(P::phase, 458, 698, 54, 24);
    place(P::mute, 454, 746, 58, 22);
    place(P::outputGain, 444, 778, 68, 180);
    place(P::meterPosition, 288, 942, 144, 22);
}

void StripAudioProcessorEditor::paint(juce::Graphics &g) {
    const auto &background = style->background;
    const auto &panel = style->panel;
    const auto &ink = style->ink;
    const auto &muted = style->muted;
    const auto &red = style->red;
    const auto &rule = style->rule;
    const auto &track = style->track;
    g.fillAll(background);
    auto section = [&g, this](int x, int y, int w, int h, const juce::String &number,
                              const juce::String &title) {
        g.setColour(style->panel);
        g.fillRect(x, y, w, h);
        g.setColour(style->rule);
        g.drawHorizontalLine(y, float(x), float(x + w));
        g.setColour(style->red);
        g.setFont(type(9, true));
        g.drawText(number, x + 12, y + 9, 22, 18, juce::Justification::centredLeft);
        g.setColour(style->ink);
        g.setFont(type(14, true));
        g.drawText(title, x + 40, y + 6, w - 52, 24, juce::Justification::centredLeft);
    };
    g.setColour(ink);
    g.setFont(type(39, true));
    g.drawText("riga", 16, 5, 120, 50, juce::Justification::centredLeft);
    g.setColour(red);
    g.fillRect(112, 40, 7, 7);
    g.setColour(ink);
    g.setFont(type(11));
    g.drawText("Tone / Dynamics", 140, 19, 122, 18, juce::Justification::centredLeft);
    g.setColour(muted);
    g.setFont(type(9));
    g.drawText("LOCAL AUDIO", 140, 36, 150, 14, juce::Justification::centredLeft);
    g.setColour(ink);
    g.drawHorizontalLine(62, 16, 524);
    section(16, 72, 248, 168, "01", "Filters");
    section(16, 252, 248, 144, "02", "High");
    section(16, 408, 248, 144, "03", "High mid");
    section(16, 564, 248, 144, "04", "Low mid");
    section(16, 720, 248, 144, "05", "Low");
    section(16, 876, 248, 88, "06", "EQ / Routing");
    section(276, 72, 248, 204, "07", "Compressor");
    section(276, 288, 248, 236, "08", "Gate / Expander");
    section(276, 536, 248, 192, "09", "Input / Colour");
    section(276, 740, 248, 224, "10", "Levels");
    const float in[]{levels.inputL, levels.inputR, levels.outputL, levels.outputR};
    const char *names[]{"IN L", "IN R", "OUT L", "OUT R"};
    constexpr float top = 794, bottom = 884, height = bottom - top;
    for (int i = 0; i < 4; ++i) {
        const int x = 298 + i * 32 + (i >= 2 ? 8 : 0);
        g.setFont(type(8.5f, true));
        g.setColour(muted);
        g.drawText(names[i], x - 10, 774, 34, 18, juce::Justification::centred);
        g.setColour(track);
        g.fillRect(x, int(top), 14, int(height));
        const float db = juce::Decibels::gainToDecibels(juce::jmax(.000001f, in[i]));
        const float fill = juce::jlimit(0.0f, 1.0f, (db + 60) / 60) * height;
        g.setColour(db > 0 ? red : ink);
        g.fillRect(float(x), bottom - fill, 14.0f, fill);
        for (int mark = 0; mark <= 10; ++mark) {
            g.setColour(panel);
            g.drawHorizontalLine(int(top + float(mark) * height / 10), float(x), float(x + 14));
        }
        g.setColour(ink);
        g.setFont(type(10));
        g.drawText(db <= -60 ? juce::String("-inf") : juce::String(db, 1), x - 12, 888, 38, 18,
                   juce::Justification::centred);
    }
    g.setColour(rule);
    g.drawVerticalLine(360, 778, 908);
    const float reductions[]{levels.compression, levels.gate};
    const char *names2[]{"COMP", "GATE"};
    for (int i = 0; i < 2; ++i) {
        const int y = 916 + i * 16;
        g.setFont(type(8, true));
        g.setColour(muted);
        g.drawText(names2[i], 288, y - 4, 36, 16, juce::Justification::centredLeft);
        g.setColour(track);
        g.fillRect(328, y, 104, 4);
        g.setColour(red);
        g.fillRect(328.0f, float(y), 104 * juce::jlimit(0.0f, 1.0f, reductions[i] / 30), 4.0f);
    }
    g.setColour(rule);
    g.drawHorizontalLine(976, 16, 524);
    g.setColour(muted);
    g.setFont(type(9));
    g.drawText("RIGA / 01", 16, 980, 120, 16, juce::Justification::centredLeft);
    g.drawText("Double-click to reset / Click a value to edit", 150, 980, 374, 16,
               juce::Justification::centredRight);
}
void StripAudioProcessorEditor::timerCallback() {
    for (auto &c : controls)
        c->refreshContext();
    levels = processor.meterValues();
    repaint(288, 774, 148, 166);
}
