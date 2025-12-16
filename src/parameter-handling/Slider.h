/*
  ==============================================================================

    Slider.h
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include "../utilities/VoidActions.h"

namespace nlt
{

class Slider : public juce::Slider
{
public:
    Slider() { setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag); }

    ~Slider() override { setLookAndFeel (nullptr); }

    template <typename Fn>
    void addValueChangedFn (Fn&& fn)
    {
        valueChangedActions.addAction (NLT_FWD (fn));
    }

    void resetValueChangedFns()
    {
        valueChangedActions.clearActions();
        resetValueChangedFnsInternal();
    }

    void valueChanged() final { valueChangedActions.triggerActions(); }

protected:
    VoidActions              valueChangedActions;
    Label*                   valueBoxPointer { nullptr };
    NormalisableRange<float> normalisedRange;

    virtual void resetValueChangedFnsInternal() {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Slider)
};

} // namespace nlt
