#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
class NormalizedParameter final : public juce::AudioParameterFloat {
  public:
    NormalizedParameter(const strip::ParamSpec &s, const juce::AudioParameterFloatAttributes &attributes,
                        std::atomic<int> &selectedBank)
        : AudioParameterFloat(
              juce::ParameterID(s.id, 1), s.name,
              juce::NormalisableRange<float>(0.0f, 1.0f, s.steps > 0 ? 1.0f / float(s.steps) : 0.0f),
              s.initial, attributes),
          initial(s.initial), intervals(s.steps), setting(juce::String(s.id) == "setting"),
          activeBank(selectedBank) {
        for (auto &value : banks)
            value.store(s.initial);
    }
    float getDefaultValue() const override {
        return initial;
    }
    float getValue() const override {
        return getBankValue(activeBank.load());
    }
    void setValue(float value) override {
        value = std::isfinite(value) ? juce::jlimit(0.0f, 1.0f, value) : getDefaultValue();
        if (setting)
            activeBank.store(static_cast<int>(std::lround(value * 3)));
        else
            setBankValue(activeBank.load(), value);
    }
    float getBankValue(int bank) const {
        return setting ? float(bank) / 3 : banks[static_cast<std::size_t>(bank)].load();
    }
    void setBankValue(int bank, float value) {
        value = std::isfinite(value) ? juce::jlimit(0.0f, 1.0f, value) : getDefaultValue();
        banks[static_cast<std::size_t>(bank)].store(convertFrom0to1(value));
    }
    int getNumSteps() const override {
        return intervals > 0 ? intervals + 1 : juce::AudioProcessor::getDefaultNumParameterSteps();
    }
    bool isDiscrete() const override {
        return intervals > 0;
    }
    bool isBoolean() const override {
        return intervals == 1;
    }

  private:
    float initial;
    int intervals;
    bool setting;
    std::atomic<int> &activeBank;
    std::array<std::atomic<float>, 4> banks;
};
} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout
StripAudioProcessor::makeLayout(std::atomic<int> &activeBank) {
    juce::AudioProcessorValueTreeState::ParameterLayout result;
    for (std::size_t i = 0; i < strip::parameterCount; ++i) {
        const auto &s = strip::parameterSpecs[i];
        const auto p = static_cast<strip::Param>(i);
        auto attrs = juce::AudioParameterFloatAttributes()
                         .withLabel(s.unit)
                         .withStringFromValueFunction(
                             [p](float value, int) { return juce::String(strip::parameterText(p, value)); })
                         .withValueFromStringFunction([p, steps = s.steps](const juce::String &text) {
                             if (text.trim().equalsIgnoreCase(juce::String(strip::parameterText(p, 0))))
                                 return 0.0f;
                             if (text.trim().equalsIgnoreCase(juce::String(strip::parameterText(p, 1))))
                                 return 1.0f;
                             if (steps > 0) {
                                 for (int i = 0; i <= steps; ++i)
                                     if (text.equalsIgnoreCase(
                                             juce::String(strip::parameterText(p, double(i) / steps))))
                                         return float(i) / float(steps);
                             }
                             double target = text.getDoubleValue();
                             if (text.containsIgnoreCase("khz"))
                                 target *= 1000;
                             double best = 0, distance = std::numeric_limits<double>::infinity();
                             const int divisions = steps > 0 ? steps : 10000;
                             for (int i = 0; i <= divisions; ++i) {
                                 const double value = double(i) / divisions;
                                 const double d = std::abs(strip::physical(p, value) - target);
                                 if (d < distance) {
                                     distance = d;
                                     best = value;
                                 }
                             }
                             return static_cast<float>(best);
                         });
        result.add(std::make_unique<NormalizedParameter>(s, attrs, activeBank));
    }
    return result;
}

StripAudioProcessor::StripAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                         .withInput("External Key", juce::AudioChannelSet::stereo(), false)),
      state(*this, nullptr, "StripState", makeLayout(activeBank)) {
    for (std::size_t i = 0; i < parameters.size(); ++i)
        parameters[i] = state.getParameter(strip::parameterSpecs[i].id);
    state.addParameterListener("setting", this);
}

bool StripAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
    const auto main = layouts.getMainInputChannelSet();
    if (main != juce::AudioChannelSet::mono() && main != juce::AudioChannelSet::stereo())
        return false;
    if (main != layouts.getMainOutputChannelSet())
        return false;
    return layouts.inputBuses.size() < 2 || layouts.inputBuses[1].isDisabled() ||
           layouts.inputBuses[1] == juce::AudioChannelSet::mono() ||
           layouts.inputBuses[1] == juce::AudioChannelSet::stereo();
}

void StripAudioProcessor::prepareToPlay(double rate, int blockSize) {
    monoScratch.resize(static_cast<std::size_t>(juce::jmax(1, blockSize)));
    engine.prepare(rate, juce::jmax(1, blockSize), getMainBusNumInputChannels());
    engine.reset();
}
void StripAudioProcessor::releaseResources() {
    engine.reset();
}
void StripAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &) {
    render(buffer, false);
}
void StripAudioProcessor::processBlockBypassed(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &) {
    render(buffer, true);
}

void StripAudioProcessor::render(juce::AudioBuffer<float> &buffer, bool hostBypass) {
    juce::ScopedNoDenormals noDenormals;
    strip::Parameters values{};
    const int bank = activeBank.load();
    for (std::size_t i = 0; i < values.size(); ++i)
        values[i] = static_cast<NormalizedParameter *>(parameters[i])->getBankValue(bank);
    if (hostBypass)
        values[strip::index(strip::Param::bypass)] = 1.0f;
    engine.setParameters(values);
    auto main = getBusBuffer(buffer, true, 0);
    auto key = getBusCount(true) > 1 ? getBusBuffer(buffer, true, 1) : juce::AudioBuffer<float>();
    const auto count = main.getNumSamples();
    if (main.getNumChannels() == 0 || monoScratch.empty())
        return;
    // Chunking also handles a host that supplies blocks larger than its declared maximum.
    const int capacity = static_cast<int>(monoScratch.size());
    for (int offset = 0; offset < count; offset += capacity) {
        const int n = juce::jmin(capacity, count - offset);
        auto *left = main.getWritePointer(0, offset);
        float *right;
        if (main.getNumChannels() > 1)
            right = main.getWritePointer(1, offset);
        else {
            std::copy_n(left, n, monoScratch.data());
            right = monoScratch.data();
        }
        const float *keyL = key.getNumChannels() > 0 ? key.getReadPointer(0, offset) : nullptr;
        const float *keyR = key.getNumChannels() > 1 ? key.getReadPointer(1, offset) : keyL;
        engine.process(left, right, keyL, keyR, n);
    }
    const auto m = engine.meters();
    const std::array<float, 6> v{m.inputL, m.inputR, m.outputL, m.outputR, m.compression, m.gate};
    for (std::size_t i = 0; i < v.size(); ++i)
        meterAtoms[i].store(v[i], std::memory_order_relaxed);
}
StripAudioProcessor::MeterValues StripAudioProcessor::meterValues() const noexcept {
    return {meterAtoms[0].load(std::memory_order_relaxed), meterAtoms[1].load(std::memory_order_relaxed),
            meterAtoms[2].load(std::memory_order_relaxed), meterAtoms[3].load(std::memory_order_relaxed),
            meterAtoms[4].load(std::memory_order_relaxed), meterAtoms[5].load(std::memory_order_relaxed)};
}
juce::AudioProcessorParameter *StripAudioProcessor::getBypassParameter() const {
    return state.getParameter("bypass");
}
StripAudioProcessor::~StripAudioProcessor() {
    state.removeParameterListener("setting", this);
    cancelPendingUpdate();
}
int StripAudioProcessor::getCurrentProgram() {
    return activeBank.load();
}
void StripAudioProcessor::setCurrentProgram(int i) {
    state.getParameter("setting")->setValueNotifyingHost(float(juce::jlimit(0, 3, i)) / 3);
    if (juce::MessageManager::getInstance()->isThisTheMessageThread()) {
        cancelPendingUpdate();
        handleAsyncUpdate();
    }
}
void StripAudioProcessor::parameterChanged(const juce::String &, float) {
    if (!restoring.load())
        triggerAsyncUpdate();
}
void StripAudioProcessor::handleAsyncUpdate() {
    if (restoring.load())
        return;
    // Bank values already changed atomically; only refresh the host and UI here.
    for (auto *parameter : parameters)
        parameter->sendValueChangedMessageToListeners(parameter->getValue());
}
void StripAudioProcessor::getStateInformation(juce::MemoryBlock &data) {
    const juce::ScopedLock lock(bankLock);
    auto root = state.copyState();
    root.removeChild(root.getChildWithName("MemoryBanks"), nullptr);
    juce::ValueTree memories("MemoryBanks");
    const int selected = activeBank.load();
    for (std::size_t i = 0; i < parameters.size(); ++i)
        root.getChildWithProperty("id", strip::parameterSpecs[i].id)
            .setProperty("value", static_cast<NormalizedParameter *>(parameters[i])->getBankValue(selected),
                         nullptr);
    for (int bank = 0; bank < 4; ++bank) {
        juce::ValueTree entry("Bank");
        for (std::size_t i = 0; i < parameters.size(); ++i)
            entry.setProperty(strip::parameterSpecs[i].id,
                              static_cast<NormalizedParameter *>(parameters[i])->getBankValue(bank), nullptr);
        memories.addChild(entry, -1, nullptr);
    }
    root.addChild(memories, -1, nullptr);
    if (auto xml = root.createXml())
        copyXmlToBinary(*xml, data);
}
void StripAudioProcessor::setStateInformation(const void *data, int size) {
    auto xml = getXmlFromBinary(data, size);
    if (!xml || !xml->hasTagName(state.state.getType()))
        return;
    const juce::ScopedLock lock(bankLock);
    restoring.store(true);
    cancelPendingUpdate();
    auto root = juce::ValueTree::fromXml(*xml);
    auto memories = root.getChildWithName("MemoryBanks");
    const float selected = root.getChildWithProperty("id", "setting").getProperty("value", 0.0f);
    activeBank.store(std::isfinite(selected) ? int(std::lround(juce::jlimit(0.0f, 1.0f, selected) * 3)) : 0);
    for (int bank = 0; bank < 4; ++bank) {
        auto entry = memories.getChild(bank);
        for (std::size_t i = 0; i < parameters.size(); ++i) {
            auto *parameter = static_cast<NormalizedParameter *>(parameters[i]);
            parameter->setBankValue(
                bank, entry.getProperty(strip::parameterSpecs[i].id, parameter->getDefaultValue()));
            if (bank == activeBank.load())
                parameter->setBankValue(bank, root.getChildWithProperty("id", strip::parameterSpecs[i].id)
                                                  .getProperty("value", parameter->getDefaultValue()));
        }
    }
    root.removeChild(memories, nullptr);
    state.replaceState(root);
    restoring.store(false);
    triggerAsyncUpdate();
}
juce::AudioProcessorEditor *StripAudioProcessor::createEditor() {
    return new StripAudioProcessorEditor(*this);
}
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new StripAudioProcessor();
}
