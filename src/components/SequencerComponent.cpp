#include "SequencerComponent.h"

struct StepBox : public Component
{
    StepBox() { setInterceptsMouseClicks (false, false); }
    void paint (Graphics& g) override
    {
        g.setColour (Colours::red);
        g.drawRect (getLocalBounds(), 2);
    }
};

SequencerComponent::SequencerComponent()
{
    setLookAndFeel (&look);
    toggleValues.resize (64, false);

    for (auto i = 0; i < 64; ++i)
    {
        auto t = toggles.add (std::make_unique<ToggleButton>());
        t->setClickingTogglesState (true);
        t->setMouseCursor (MouseCursor::PointingHandCursor);
        t->onClick = [this, t, i]()
        {
            if (!blockSequenceEditOutput)
            {
                toggleValues[i] = t->getToggleState();
                sendSynchronousChangeMessage();
            }
        };
        if (i % 4 == 0)
            t->setColour (ToggleButton::textColourId, Colours::lightgrey);
        addAndMakeVisible (t);
    }

    stepBox = std::make_unique<StepBox>();
    addAndMakeVisible (*stepBox);
}

SequencerComponent::~SequencerComponent()
{
    setLookAndFeel (nullptr);
}

void SequencerComponent::paint (Graphics& g)
{
    //    g.setColour (Colours::black);
    //    g.fillRect (getLocalBounds());
}

void SequencerComponent::resized()
{
    auto bounds       = getLocalBounds().toFloat();
    auto toggleWidth  = bounds.getWidth() / (float) jmax (numSteps, 1);
    auto toggleHeight = bounds.getHeight();
    for (auto i = 0; i < numSteps; ++i)
    {
        auto toggleBounds = Rectangle<float> ((float) i * toggleWidth, 0, toggleWidth, toggleHeight);
        toggles[i]->setBounds (toggleBounds.toNearestInt().reduced (3, 1));
    }
}

void SequencerComponent::setSequenceLength (int newNumSteps)
{
    if (numSteps != newNumSteps)
    {
        numSteps = newNumSteps;

        for (auto i = 0; i < toggles.size(); ++i)
            toggles[i]->setVisible (i < numSteps);

        resized();
    }
}

void SequencerComponent::setSequenceWithEvent (const RNBO::MessageEvent& event)
{
    auto   melodyList = event.getListValue().get();
    size_t length     = melodyList->length;
    jassert (length <= toggleValues.size());

    for (auto i = 0; i < length; ++i)
        toggles[i]->setInterceptsMouseClicks (false, false);

    {
        ScopedValueSetter<bool> scopedEditBlocker { blockSequenceEditOutput, true };
        for (auto i = 0; i < length; ++i)
        {
            toggleValues[i] = static_cast<bool> ((*melodyList)[i]);
            toggles[i]->setToggleState (toggleValues[i], juce::sendNotificationSync);
        }
        setSequenceLength (numSteps);
    }
    for (auto i = 0; i < length; ++i)
        toggles[i]->setInterceptsMouseClicks (true, true);
}

std::vector<bool> SequencerComponent::getCurrentSequence() const
{
    auto first = toggleValues.begin();
    auto last  = toggleValues.end();
    return { first, last };
}

void SequencersContainer::add (RNBO::MessageTag tag)
{
    auto s            = sequencers.add (std::make_unique<SequencerComponent>());
    sequencerMap[tag] = s;
}

SequencerStepIndicator::SequencerStepIndicator()
{
    setInterceptsMouseClicks (false, false);
}

void SequencerStepIndicator::resized()
{
    setCurrentStep (currentStep);
}

void SequencerStepIndicator::paint (Graphics& g)
{
    auto bounds        = getLocalBounds();
    auto toggleWidth   = (float) bounds.getWidth() / (float) jmax (numSteps, 1);
    auto toggleHeight  = 20;
    auto toggleBounds  = Rectangle<float> ((float) currentStep * toggleWidth, 0, toggleWidth, toggleHeight);
    auto centre        = toggleBounds.getCentre();
    auto pointerBounds = toggleBounds.withWidth (toggleHeight).withCentre (centre).withHeight (15);
    Path p;
    p.startNewSubPath (pointerBounds.getTopLeft());
    p.lineTo (pointerBounds.getCentreX(), pointerBounds.getBottom());
    p.lineTo (pointerBounds.getTopRight());
    p.closeSubPath();
    g.setColour (Colours::darkgrey);
    g.fillPath (p);
    g.setColour (Colours::black);
    g.drawVerticalLine (centre.getX(), pointerBounds.getBottom(), getBottom());
    g.strokePath (p, PathStrokeType (2.0f));
    g.setFont (Font (16.0f).withExtraKerningFactor (0.05f));
    toggleBounds.setPosition (0, 17);
    for (auto i = 0; i < numSteps; ++i)
    {
        if (i % 4 == 0)
            g.drawFittedText (String (i + 1),
                              toggleBounds.withX ((float) i * toggleWidth).toNearestInt(),
                              Justification::centred,
                              1);
    }
}

void SequencerStepIndicator::setCurrentStep (int newCurrentStep)
{
    if (currentStep != newCurrentStep)
    {
        currentStep = newCurrentStep;
        repaint();
    }
}

void SequencerStepIndicator::setSequenceLength (int newNumSteps)
{
    if (numSteps != newNumSteps)
    {
        numSteps = newNumSteps;

        repaint();
    }
}
