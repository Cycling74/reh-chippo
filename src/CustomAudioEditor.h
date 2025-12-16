#pragma once
#include "JuceHeader.h"
#include "RNBO.h"
#include "CustomAudioProcessor.h"
#include "components/Components.h"
#include "utilities/TimerAction.h"
#include "utilities/change-listeners/ChangeListenerActions.h"
#include "parameter-handling/APVTSCallback.h"
#include "parameter-handling/APVTSControl.h"
#include "components/PresetBar/PresetBar.h"

using ParamSliderLinearVertical = nlt::APVTSControl<SliderMasked>;
using ParamSliderRotary         = nlt::APVTSControl<SliderRotary>;
using ParamImageButton          = nlt::APVTSControl<ImageButton>;
using ParamToggle               = nlt::APVTSControl<ToggleButton>;
using ParamBox                  = nlt::APVTSControl<ComboBox>;

class CustomAudioEditor : public juce::AudioProcessorEditor,
                          private juce::AudioProcessorListener,
                          public juce::AsyncUpdater,
                          public RNBO::EventHandler
{
public:
    CustomAudioEditor (CustomAudioProcessor* p, RNBO::CoreObject& rnboObject);
    ~CustomAudioEditor() override;
    void resized() override;

    void handleMessageEvent (const RNBO::MessageEvent& event) override;
    void handleAsyncUpdate() override { drainEvents(); }
    void eventsAvailable() override { this->triggerAsyncUpdate(); }

private:
    void audioProcessorChanged (juce::AudioProcessor*, const ChangeDetails&) override {}
    void audioProcessorParameterChanged (juce::AudioProcessor*, int parameterIndex, float) override;

protected:
    CustomAudioProcessor*     _audioProcessor;
    RNBO::JuceAudioProcessor* rnboProcessor;
    RNBO::CoreObject&         _rnboObject;
    PresetBar                 presetBar;
    EditorBackground          editorBG;
    AboutPanel                aboutPanel;
    SequencerComponent        melodySequencer;
    SequencerComponent        bassSequencer;
    SequencerComponent        kickSequencer;
    SequencerComponent        snareSequencer;
    SequencerComponent        hatSequencer;
    SequencersContainer       sequencersContainer;
    SequencerStepIndicator    seqStepIndicator;
    ImageButton               aboutPanelButton;

    enum SequencerTracks
    {
        melody,
        bass,
        hat,
        snare,
        kick
    };
    int                                    currentStep { 0 };
    juce::ValueTree                        pluginState;
    RNBO::ParameterEventInterfaceUniquePtr _parameterInterface;
    nlt::TimerAction                       currentStepAction;
    nlt::ChangeListenerActions             sequenceEditActions;
    SharedResourcePointer<TooltipWindow>   tooltipWindow;

    std::map<Identifier, std::unique_ptr<ImageButton>> seqGenButtons;
    std::map<Identifier, std::unique_ptr<Label>>       seqGenLabels;

    nlt::APVTSCallbacks                                callbacks;
    std::map<Identifier, std::unique_ptr<nlt::Slider>> sliders;
    std::map<Identifier, std::unique_ptr<Label>>       sliderLabels;

    std::unique_ptr<ParamToggle>      runToggle;
    Label                             runLabel { "", "GO" };
    std::unique_ptr<ParamImageButton> infinityToggle;
    ChippoLook::LinearBarVerticalLook linearBarVerticalLook;

    std::unique_ptr<ParamBox> scaleBox;
    juce::Label               scaleBoxLabel { "scale label", "SCALE" };

    Image bgImage;

    void setupSliders();
    void setupSequencers();
    void setupToggles();
    void setupButtons();
    void setupTooltips();

    void sendSequencerValues (SequencerComponent& seq, juce::StringRef inputTag);

    SET_DEFAULT_LOOK_AND_FEEL (ChippoLook::Look)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
