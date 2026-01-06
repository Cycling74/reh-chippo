#include "CustomAudioEditor.h"
#include "BinaryData.h"
#include "utilities/MidiNoteNumberFromName.h"

using namespace juce;

CustomAudioEditor::CustomAudioEditor (CustomAudioProcessor* const p, RNBO::CoreObject& rnboObject)
    : AudioProcessorEditor (p)
    , _audioProcessor (p)
    , editorContainer (p, rnboObject)
{
    addAndMakeVisible (editorContainer);

    changeListenerActions.add (editorContainer,
                               [this] (auto* broadcaster)
                               {
                                   if (auto* parent = getParentComponent())
                                       parent->setSize (editorContainer.getWidth() * editorContainer.getScale(),
                                                        editorContainer.getHeight() * editorContainer.getScale());
                                   setSize (editorContainer.getWidth() * editorContainer.getScale(),
                                            editorContainer.getHeight() * editorContainer.getScale());
                               });
    setSize (editorContainer.getWidth() * editorContainer.getScale(),
             editorContainer.getHeight() * editorContainer.getScale());
    if (auto* parent = getParentComponent())
        parent->setSize (getWidth(), getHeight());
}

CustomAudioEditor::~CustomAudioEditor()
{
    setLookAndFeel (nullptr);
}
