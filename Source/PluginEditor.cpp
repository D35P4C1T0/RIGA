#include "PluginEditor.h"
#include <cmath>

namespace {
const juce::Colour background(0xffeeede7), panel(0xfffaf9f5), ink(0xff202320), muted(0xff70736c);
const juce::Colour red(0xffdf3c2f), rule(0xffd4d5cc), track(0xffdedfd6);
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

class StripAudioProcessorEditor::Style final : public juce::LookAndFeel_V4 {
  public:
    Style() {
        setColour(juce::Label::textColourId, ink);
        setColour(juce::ComboBox::backgroundColourId, panel);
        setColour(juce::ComboBox::outlineColourId, rule);
        setColour(juce::ComboBox::textColourId, ink);
        setColour(juce::PopupMenu::backgroundColourId, panel);
        setColour(juce::PopupMenu::textColourId, ink);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, red);
        setColour(juce::PopupMenu::highlightedTextColourId, panel);
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
        label->setColour(juce::Label::textColourId, ink);
        label->setFont(type(11.5f, true));
        label->setColour(juce::TextEditor::textColourId, ink);
        label->setColour(juce::TextEditor::backgroundColourId, panel);
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
        g.setColour(colour.getPerceivedBrightness() > .65f ? ink : panel);
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
        g.setColour(active ? panel : ink);
        g.setFont(type(9.5f, true));
        g.drawFittedText(button.getButtonText(), r.toNearestInt().reduced(4, 1), juce::Justification::centred,
                         2);
    }
    void drawComboBox(juce::Graphics &g, int w, int h, bool, int, int, int, int, juce::ComboBox &) override {
        auto r = juce::Rectangle<float>(.5f, .5f, float(w) - 1, float(h) - 1);
        g.setColour(panel);
        g.fillRect(r);
        g.setColour(rule);
        g.drawRect(r, .7f);
        juce::Path arrow;
        arrow.startNewSubPath(float(w) - 15, float(h) * .45f);
        arrow.lineTo(float(w) - 11, float(h) * .6f);
        arrow.lineTo(float(w) - 7, float(h) * .45f);
        g.setColour(ink);
        g.strokePath(arrow, juce::PathStrokeType(1));
    }
    void positionComboBoxText(juce::ComboBox &box, juce::Label &label) override {
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
            slider.setColour(juce::Slider::textBoxTextColourId, ink);
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
    setSize(860, 736);
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
    place(P::setting, 644, 10, 80, 44);
    place(P::bypass, 740, 24, 104, 26);
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
        place(comp[i], 28 + (i % 3) * 72, 280 + (i / 3) * 74, 72, 72);
    place(P::compFast, 28, 430, 104, 22);
    place(P::compLink, 136, 430, 104, 22);
    place(P::gateRange, 28, 496, 72, 72);
    place(P::gateThreshold, 100, 496, 72, 72);
    place(P::gateRelease, 172, 496, 72, 72);
    place(P::hysteresis, 28, 572, 72, 72);
    place(P::gateFast, 106, 576, 134, 24);
    place(P::gateMode, 106, 606, 84, 24);
    place(P::gateInvert, 194, 606, 46, 24);
    place(P::gateThresholdRange, 28, 650, 104, 24);
    place(P::vcaMode, 136, 650, 104, 24);
    place(P::externalKey, 28, 678, 104, 22);
    place(P::dynamicsOn, 136, 678, 104, 22);
    place(P::highGain, 292, 106, 104, 82);
    place(P::highFreq, 408, 106, 104, 82);
    place(P::highBell, 438, 192, 78, 22);
    place(P::highMidGain, 292, 262, 72, 96);
    place(P::highMidFreq, 364, 262, 72, 96);
    place(P::highMidQ, 436, 262, 72, 96);
    place(P::lowMidGain, 292, 418, 72, 96);
    place(P::lowMidFreq, 364, 418, 72, 96);
    place(P::lowMidQ, 436, 418, 72, 96);
    place(P::lowGain, 292, 568, 104, 84);
    place(P::lowFreq, 408, 568, 104, 84);
    place(P::lowBell, 438, 542, 78, 22);
    place(P::eqType, 292, 680, 84, 24);
    place(P::eqOn, 382, 680, 50, 24);
    place(P::eqPosition, 440, 668, 76, 36);
    place(P::inputGain, 556, 106, 88, 96);
    place(P::noise, 652, 106, 88, 96);
    place(P::drive, 748, 106, 88, 96);
    place(P::channel, 556, 210, 130, 40);
    place(P::monitor, 698, 210, 138, 40);
    place(P::stereoMode, 556, 262, 130, 24);
    place(P::power, 698, 262, 64, 24);
    place(P::phase, 768, 262, 68, 24);
    place(P::mute, 778, 82, 58, 22);
    place(P::outputGain, 760, 342, 76, 356);
    place(P::meterPosition, 556, 676, 180, 22);
}

void StripAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(background);
    auto section = [&](int x, int y, int w, int h, const juce::String &number, const juce::String &title) {
        g.setColour(panel);
        g.fillRect(x, y, w, h);
        g.setColour(rule);
        g.drawHorizontalLine(y, float(x), float(x + w));
        g.setColour(red);
        g.setFont(type(9, true));
        g.drawText(number, x + 12, y + 9, 22, 18, juce::Justification::centredLeft);
        g.setColour(ink);
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
    g.drawText("Tone / Dynamics", 156, 19, 250, 18, juce::Justification::centredLeft);
    g.setColour(muted);
    g.setFont(type(9));
    g.drawText("LOCAL AUDIO", 156, 36, 250, 14, juce::Justification::centredLeft);
    g.setColour(ink);
    g.drawHorizontalLine(62, 16, 844);
    section(16, 72, 248, 168, "01", "Filters");
    section(16, 252, 248, 204, "02", "Compressor");
    section(16, 468, 248, 236, "03", "Gate / Expander");
    section(280, 72, 248, 144, "04", "High");
    section(280, 228, 248, 144, "05", "High mid");
    section(280, 384, 248, 144, "06", "Low mid");
    section(280, 540, 248, 116, "07", "Low");
    section(544, 72, 300, 228, "08", "Input / Colour");
    section(544, 312, 300, 392, "09", "Levels / Output");
    const float in[]{levels.inputL, levels.inputR, levels.outputL, levels.outputR};
    const char *names[]{"IN L", "IN R", "OUT L", "OUT R"};
    constexpr float top = 384, bottom = 606, height = bottom - top;
    for (int i = 0; i < 4; ++i) {
        const int x = 568 + i * 44;
        g.setFont(type(8.5f, true));
        g.setColour(muted);
        g.drawText(names[i], x - 10, 358, 34, 18, juce::Justification::centred);
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
        g.drawText(db <= -60 ? juce::String("-inf") : juce::String(db, 1), x - 12, 612, 38, 18,
                   juce::Justification::centred);
    }
    g.setColour(rule);
    g.drawVerticalLine(642, 354, 632);
    const float reductions[]{levels.compression, levels.gate};
    const char *names2[]{"COMP", "GATE"};
    for (int i = 0; i < 2; ++i) {
        const int y = 640 + i * 18;
        g.setFont(type(8, true));
        g.setColour(muted);
        g.drawText(names2[i], 556, y - 4, 36, 16, juce::Justification::centredLeft);
        g.setColour(track);
        g.fillRect(596, y, 138, 4);
        g.setColour(red);
        g.fillRect(596.0f, float(y), 138 * juce::jlimit(0.0f, 1.0f, reductions[i] / 30), 4.0f);
    }
    g.setColour(rule);
    g.drawHorizontalLine(716, 16, 844);
    g.setColour(muted);
    g.setFont(type(9));
    g.drawText("RIGA / 01", 16, 718, 170, 16, juce::Justification::centredLeft);
    g.drawText("Double-click to reset / Click a value to edit", 400, 718, 444, 16,
               juce::Justification::centredRight);
}
void StripAudioProcessorEditor::timerCallback() {
    for (auto &c : controls)
        c->refreshContext();
    levels = processor.meterValues();
    repaint(550, 350, 192, 320);
}
