/*
  ==============================================================================

    PresetBar.h


  ==============================================================================
*/

#include "PresetBar.h"

using namespace juce;

/**
 to use this correctly make sure you set the xml save data tag (used in get/setStateInformation)
 as a property value in your app properties object. use this identifier to set the value key
 */
inline static Identifier saveDataTagIdt { "SaveDataAppTag" };

/**
 * Use this to store the name of the currently used preset. This should probably be entirely internal to the toolbar
 */
inline static Identifier currentPresetNameIdt { "CurrentPresetName" };

/**
 * Use this to store the name of the currently used preset. This should probably be entirely internal to the toolbar
 */
inline static Identifier currentPresetFileLocationIdt { "CurrentPresetFileLocation" };

PresetBar::PresetBar (File _location, ValueTree pluginState)
    : presetLocation (_location)
    , presetTree (pluginState)
{
    preset.setButtonText ("untitled");
    preset.setMouseCursor (MouseCursor::PointingHandCursor);
    preset.onClick = [this]()
    {
        PopupMenu pm;

        populateMenu (pm, presetLocation);

        pm.showMenuAsync (PopupMenu::Options().withTargetComponent (preset));
    };
    addAndMakeVisible (preset);

    saveAsButton.onClick = [this]()
    {
        launchFileChooser ("save preset as",
                           presetLocation.getChildFile ("user"),
                           "*." + fileExtension,
                           FileBrowserComponent::FileChooserFlags::canSelectFiles
                               | FileBrowserComponent::FileChooserFlags::saveMode,
                           [this] (auto& chooser)
                           {
                               File file = chooser.getResult();
                               if (file.getFullPathName().isEmpty())
                                   return;
                               currentFile = file.getFullPathName();
                               currentFile.create();
                               savePreset (currentFile);
                               setPresetNameAndFile (currentFile.getFileNameWithoutExtension(), currentFile);
                           });
    };

    saveAsButton.setMouseCursor (MouseCursor::PointingHandCursor);
    addAndMakeVisible (saveAsButton);

    saveButton.onClick = [this]()
    {
        if (currentFile.getFullPathName().isEmpty())
        {
            saveAsButton.triggerClick();
            return;
        }
        savePreset (currentFile);
    };
    saveButton.setMouseCursor (MouseCursor::PointingHandCursor);
    addAndMakeVisible (saveButton);

    loadButton.onClick = [this]()
    {
        launchFileChooser ("load preset",
                           presetLocation,
                           "*." + fileExtension,
                           FileBrowserComponent::FileChooserFlags::canSelectFiles
                               | FileBrowserComponent::FileChooserFlags::openMode,
                           [this] (auto& chooser)
                           {
                               File file = chooser.getResult();
                               if (file.existsAsFile() && file.getFileExtension().contains (fileExtension))
                               {
                                   currentFile = file.getFullPathName();
                                   MemoryBlock mem;
                                   currentFile.loadFileAsData (mem);
                                   loadPresetFn (mem);
                                   setPresetNameAndFile (currentFile.getFileNameWithoutExtension(), currentFile);
                               }
                           });
    };
    loadButton.setMouseCursor (MouseCursor::PointingHandCursor);
    addAndMakeVisible (loadButton);

    setupPresetTree();

    auto setupArrows = [this] (ImageButton& button, bool isUpButton)
    {
        button.setMouseCursor (MouseCursor::PointingHandCursor);
        button.onClick = [this, isUpButton]() { useIncDecButton (isUpButton); };
    };

    auto upImage = ImageCache::getFromMemory (BinaryData::arrow_up_png, BinaryData::arrow_up_pngSize);
    upButton.setImages (false,
                        true,
                        false,
                        upImage,
                        1.0f,
                        Colours::transparentBlack,
                        upImage,
                        1.0f,
                        Colours::black.withAlpha (0.3f),
                        upImage,
                        1.0f,
                        Colours::black.withAlpha (0.6f));
    setupArrows (upButton, true);
    addAndMakeVisible (upButton);

    auto downImage = ImageCache::getFromMemory (BinaryData::arrow_down_png, BinaryData::arrow_down_pngSize);
    downButton.setImages (false,
                          true,
                          false,
                          downImage,
                          1.0f,
                          Colours::transparentBlack,
                          downImage,
                          1.0f,
                          Colours::black.withAlpha (0.3f),
                          downImage,
                          1.0f,
                          Colours::black.withAlpha (0.6f));
    setupArrows (downButton, false);
    addAndMakeVisible (downButton);
}

void PresetBar::resized()
{
    auto bounds       = getLocalBounds();
    auto incDecBounds = bounds.removeFromRight (50);
    upButton.setBounds (getWidth() - 45, 0, 35, getHeight() * 0.45f);
    downButton.setBounds (getWidth() - 45, getHeight() * 0.55f, 35, getHeight() * 0.45f);
    preset.setBounds (bounds.removeFromTop (40));
    loadButton.setBounds (bounds.removeFromRight (50));
    saveAsButton.setBounds (bounds.removeFromRight (75));
    saveButton.setBounds (bounds.removeFromRight (50));
}

void PresetBar::savePreset (File file)
{
    if (file != currentFile)
        currentFile = file;

    if (currentFile.existsAsFile())
    {
        if (currentFile.getFileName().isNotEmpty())
        {
            String s (currentFile.getFileNameWithoutExtension());
            setPresetNameAndFile (s, currentFile);

            MemoryBlock block;
            block.reset();
            if (savePresetFn != nullptr)
                savePresetFn (block);
            else
                jassertfalse; // must set save function !

            currentFile.replaceWithData (block.getData(), block.getSize());
        }
    }
    else
        saveAsButton.triggerClick();
}

void PresetBar::loadPresetInternal (const juce::File& file)
{
    MemoryBlock mem;
    file.loadFileAsData (mem);
    if (this->loadPresetFn != nullptr)
        this->loadPresetFn (mem);
    else
        jassertfalse; // need to set the load function!

    this->preset.setButtonText (file.getFileNameWithoutExtension());
    this->currentFile = file;
}

void PresetBar::populateMenu (PopupMenu& pm, File folder)
{
    auto folders = folder.findChildFiles (File::TypesOfFileToFind::findDirectories, false);
    for (auto i = 0; i < folders.size(); ++i)
    {
        PopupMenu subPm;
        populateMenu (subPm, folders[i]);
        pm.addSubMenu (folders[i].getFileName(), subPm);
    }
    auto files = folder.findChildFiles (File::TypesOfFileToFind::findFiles, false, "*." + fileExtension);
    for (auto i = 0; i < files.size(); ++i)
    {
        pm.addItem (files[i].getFileNameWithoutExtension(), [this, file = files[i]]() { loadPresetInternal (file); });
    }
    pm.addSeparator();
    pm.addItem ("open location", [folder]() { folder.revealToUser(); });
}

void PresetBar::useIncDecButton (bool isUpButton)
{
    if (!currentFile.exists())
    {
        auto files = presetLocation.findChildFiles (File::TypesOfFileToFind::findFiles, false, "*." + fileExtension);
        if (files.isEmpty())
            return;

        auto file = files[0];
        loadPresetInternal (file);
        return;
    }

    auto files =
        currentFile.getParentDirectory().findChildFiles (File::TypesOfFileToFind::findFiles, false, "*." + fileExtension);

    if (files.isEmpty())
        return;

    for (auto i = 0; i < files.size(); ++i)
    {
        if (currentFile == files[i])
        {
            auto newIndex = i + (isUpButton ? -1 : 1);
            if (newIndex < 0)
                newIndex += files.size();
            loadPresetInternal (files[newIndex % files.size()]);
            break;
        }
    }
}

void PresetBar::setPresetNameAndFile (const String& name, const File& file)
{
    if (presetTree.isValid())
    {
        presetTree.setProperty (currentPresetNameIdt, name, nullptr);
        presetTree.setProperty (currentPresetFileLocationIdt, file.getFullPathName(), nullptr);
    }
    else
        preset.setButtonText (name);
}

void PresetBar::setupPresetTree()
{
    if (!presetTree.hasProperty (currentPresetNameIdt))
        presetTree.setProperty (currentPresetNameIdt, "No Preset", nullptr);

    if (!presetTree.hasProperty (currentPresetFileLocationIdt))
        presetTree.setProperty (currentPresetNameIdt, {}, nullptr);

    vtCallbacks.add (presetTree,
                     currentPresetFileLocationIdt,
                     [this] (const var& filePath)
                     {
                         auto f = File (filePath.toString());
                         if (f.existsAsFile())
                             currentFile = f;
                     });
    vtCallbacks.add (presetTree,
                     currentPresetNameIdt,
                     [this] (const var& name)
                     {
                         if (name.toString().isNotEmpty())
                             preset.setButtonText (name.toString());
                     });
}
