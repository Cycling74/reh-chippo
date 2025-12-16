#include "CustomAudioEditor.h"
#include <json/json.hpp>

#ifdef RNBO_INCLUDE_DESCRIPTION_FILE
#    include <rnbo_description.h>
#endif

//create an instance of our custom plugin, optionally set description, presets and binary data (datarefs)
CustomAudioProcessor* CustomAudioProcessor::CreateDefault()
{
    nlohmann::json patcher_desc, presets;
#ifdef RNBO_BINARY_DATA_STORAGE_NAME
    extern RNBO::BinaryDataImpl::Storage RNBO_BINARY_DATA_STORAGE_NAME;
    RNBO::BinaryDataImpl::Storage        dataStorage = RNBO_BINARY_DATA_STORAGE_NAME;
#else
    RNBO::BinaryDataImpl::Storage dataStorage;
#endif
    RNBO::BinaryDataImpl data (dataStorage);

#ifdef RNBO_INCLUDE_DESCRIPTION_FILE
    patcher_desc = RNBO::patcher_description;
    presets      = RNBO::patcher_presets;
#endif
    return new CustomAudioProcessor (patcher_desc, presets, data);
}

CustomAudioProcessor::CustomAudioProcessor (const nlohmann::json&   patcher_desc,
                                            const nlohmann::json&   presets,
                                            const RNBO::BinaryData& data)
    : RNBO::JuceAudioProcessor (patcher_desc, presets, data)
{
    PropertiesFile::Options options;
    options.applicationName     = "Chippo";
    options.filenameSuffix      = ".config";
    options.osxLibrarySubFolder = "Application Support";

    appProperties.setStorageParameters (options);
}

juce::AudioProcessorEditor* CustomAudioProcessor::createEditor()
{
    return new CustomAudioEditor (this, this->_rnboObject);
}

void CustomAudioProcessor::handleMessageEvent (const RNBO::MessageEvent& event)
{
    RNBO::JuceAudioProcessor::handleMessageEvent (event);
}

void CustomAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto rnboPreset    = _rnboObject.getPresetSync();
    auto rnboPresetStr = RNBO::convertPresetToJSON (*rnboPreset);
    auto presetJSON    = nlohmann::json::parse (rnboPresetStr);

    presetJSON["presetName"]         = presetTree.getProperty ("CurrentPresetName").toString().toStdString();
    presetJSON["presetFileLocation"] = presetTree.getProperty ("CurrentPresetFileLocation").toString().toStdString();

    MemoryOutputStream stream (destData, false);
    stream.writeString (nlohmann::to_string (presetJSON));
}

void CustomAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::String rnboPresetStr = juce::String::createStringFromData (data, sizeInBytes);
    auto         presetJSON    = nlohmann::json::parse (rnboPresetStr.toStdString());

    if (presetJSON.contains ("CurrentPresetName"))
    {
        presetTree.setProperty ("CurrentPresetName", presetJSON["presetName"], nullptr);
        presetJSON.erase ("CurrentPresetName");
        presetTree.setProperty ("CurrentPresetFileLocation", presetJSON["presetFileLocation"], nullptr);
        presetJSON.erase ("CurrentPresetFileLocation");
    }

    auto rnboPreset = RNBO::convertJSONToPreset (nlohmann::to_string (presetJSON));

    _rnboObject.setPresetSync (std::move (rnboPreset));

    static RNBO::MessageTag retrieveSequences { RNBO::TAG ("retrieveSequences") };
    static RNBO::MessageTag bang { RNBO::TAG ("") };
    _rnboObject.sendMessage (retrieveSequences, bang);
    // now let us get all parameter updates that were triggered by the preset update immediately
    drainEvents();
}

bool CustomAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    return true;
}
