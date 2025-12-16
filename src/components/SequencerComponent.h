/*
==============================================================================

    SequencerComponent.h

==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "RNBO.h"
#include "LookAndFeel/ChippoLookAndFeel.h"

struct SequencerComponent : public Component, public ChangeBroadcaster
{
    SequencerComponent();
    ~SequencerComponent() override;

    void paint (Graphics& g) override;
    void resized() override;

    void setSequenceWithEvent (const RNBO::MessageEvent& event);
    void setSequenceLength (int newNumSteps);

    std::vector<bool> getCurrentSequence() const;

private:
    ChippoLook::SequencerToggleLook look;
    OwnedArray<ToggleButton>        toggles;
    std::vector<bool>               toggleValues;
    int                             numSteps { 8 };
    std::unique_ptr<Component>      stepBox;
    bool                            blockSequenceEditOutput { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerComponent)
};

struct SequencersContainer
{
    SequencersContainer() = default;

    void add (RNBO::MessageTag tag);

    inline SequencerComponent* operator[] (RNBO::MessageTag tag) const noexcept { return sequencerMap.at (tag); }

private:
    OwnedArray<SequencerComponent>                  sequencers;
    std::map<RNBO::MessageTag, SequencerComponent*> sequencerMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencersContainer)
};

struct SequencerStepIndicator : public Component
{
    SequencerStepIndicator();

    void setSequenceLength (int newNumSteps);
    void setCurrentStep (int newCurrentStep);

    void resized() override;
    void paint (Graphics&) override;

private:
    int currentStep { 0 };
    int numSteps { 8 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerStepIndicator)
};
