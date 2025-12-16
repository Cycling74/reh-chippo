/*
  ==============================================================================

    PresetBar.h


  ==============================================================================
*/

#pragma once
#include "../../utilities/FileChooserHolder.h"
#include "../../utilities/ValueTreeCallback.h"

struct PresetBar : public juce::Component, public FileChooserHolder
{
    PresetBar (juce::File _location, juce::ValueTree pluginState);

    void setSaveLocation (File location) { presetLocation = location; }

    template <typename Fn>
    void setSaveFn (Fn&& fn)
    {
        savePresetFn = NLT_FWD (fn);
    }

    template <typename Fn>
    void setLoadFn (Fn&& fn)
    {
        loadPresetFn = NLT_FWD (fn);
    }

    void setFileExtension (const String& extension) { fileExtension = extension; }

    void resized() override;

private:
    File                               presetLocation;
    File                               currentFile;
    TextButton                         preset;
    TextButton                         saveButton { "save" };
    std::function<void (MemoryBlock&)> savePresetFn { nullptr };
    TextButton                         saveAsButton { "save as" };
    TextButton                         loadButton { "load" };
    std::function<void (MemoryBlock&)> loadPresetFn { nullptr };
    String                             fileExtension { ".nlt" };
    std::unique_ptr<FileChooser>       fileChooser;
    ValueTree                          presetTree;
    ValueTreeCallbacks                 vtCallbacks;
    ImageButton                        upButton;
    ImageButton                        downButton;

    void populateMenu (PopupMenu& pm, File folder);

    void savePreset (File file);

    void setupPresetTree();

    void setPresetNameAndFile (const String& name, const File& file);

    void useIncDecButton (bool isUpButton);

    void loadPresetInternal (const File&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBar)
};
