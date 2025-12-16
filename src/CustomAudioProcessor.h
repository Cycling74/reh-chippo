#include "RNBO.h"
#include "RNBO_Utils.h"
#include "RNBO_JuceAudioProcessor.h"
#include "RNBO_BinaryData.h"
#include <json/json.hpp>
#include <JuceHeader.h>

class CustomAudioProcessor : public RNBO::JuceAudioProcessor
{
public:
    static CustomAudioProcessor* CreateDefault();
    CustomAudioProcessor (const nlohmann::json& patcher_desc, const nlohmann::json& presets, const RNBO::BinaryData& data);
    juce::AudioProcessorEditor* createEditor() override;

    void handleMessageEvent (const RNBO::MessageEvent& event) override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    friend class CustomAudioEditor;

private:
    juce::OwnedArray<RNBO::list> sequences;
    juce::ValueTree              presetTree { "presetTree" };
    juce::ApplicationProperties  appProperties;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioProcessor)
};
