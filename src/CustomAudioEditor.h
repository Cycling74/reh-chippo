#pragma once
#include "components/EditorContainer/EditorContainer.h"

class CustomAudioEditor : public juce::AudioProcessorEditor
{
public:
    CustomAudioEditor (CustomAudioProcessor* p, RNBO::CoreObject& rnboObject);
    ~CustomAudioEditor() override;

protected:
    CustomAudioProcessor*     _audioProcessor;
    EditorContainer            editorContainer;
    nlt::ChangeListenerActions changeListenerActions;

    SET_DEFAULT_LOOK_AND_FEEL (ChippoLook::Look)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
