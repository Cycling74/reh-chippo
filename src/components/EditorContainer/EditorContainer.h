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
#include "parameter-handling/ValueTreeToggleButton.h"

using ParamSliderLinearVertical = nlt::APVTSControl<SliderMasked>;
using ParamSliderRotary         = nlt::APVTSControl<SliderRotary>;
using ParamImageButton          = nlt::APVTSControl<ImageButton>;
using ParamToggle               = nlt::APVTSControl<ToggleButton>;
using ParamBox                  = nlt::APVTSControl<ComboBox>;

class EditorContainer : public juce::Component,
                        public juce::AsyncUpdater,
                        public RNBO::EventHandler,
                        public juce::ChangeBroadcaster
{
public:
    EditorContainer (CustomAudioProcessor* p, RNBO::CoreObject& rnboObject);
    ~EditorContainer() override;
    void resized() override;

    void handleMessageEvent (const RNBO::MessageEvent& event) override;
    void handleAsyncUpdate() override { drainEvents(); }
    void eventsAvailable() override { this->triggerAsyncUpdate(); }

    float getScale() const { return scale; }

protected:
    CustomAudioProcessor*     _audioProcessor;
    RNBO::JuceAudioProcessor* rnboProcessor;
    RNBO::CoreObject&         _rnboObject;
    ApplicationProperties&    appProperties;
    juce::ValueTree           presetTree;
    PresetBar                 presetBar;
    EditorBackground          editorBG;
    AboutPanel                aboutPanel;
    SequencerComponent        melodySequencer { "MEL" };
    SequencerComponent        bassSequencer { "BASS" };
    SequencerComponent        kickSequencer { "KICK" };
    SequencerComponent        snareSequencer { "SNR" };
    SequencerComponent        hatSequencer { "HAT" };
    // these are in the order that they appear in the editor
    std::vector<SequencerComponent*> sequencers { &melodySequencer, &bassSequencer, &hatSequencer, &snareSequencer, &kickSequencer };
    SequencerStepIndicator    seqStepIndicator;
    ImageButton               aboutPanelButton;
    TextButton                zoomButton { "zoom" };
    float                     scale { 1.0f };

    OwnedArray<nlt::ValueTreeToggleButton> seqVisToggles;
    OwnedArray<Label>                      seqVisLabels;
    nlt::ValueTreeCallbacks                vtCallbacks;

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
    void setScale (float newScale);

    void sendSequencerValues (SequencerComponent& seq, juce::StringRef inputTag);

    void setSizeFromSequencers();

    SET_DEFAULT_LOOK_AND_FEEL (ChippoLook::Look)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorContainer)
};
