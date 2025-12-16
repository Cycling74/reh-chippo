/*
  ==============================================================================

    APVTSSliderAttachment.h
    Author:  Dave Sanchez

  ==============================================================================
*/
#pragma once
#include "APVTSParameterAttachment.h"
#include "../Slider.h"

namespace nlt
{

struct APVTSSliderAttachment
{
    APVTSSliderAttachment (RangedAudioParameter& parameterInfo, nlt::Slider& s)
        : slider (s)
        , attachment (parameterInfo, [this] (float f) { setValue (f); })
    {
        slider.valueFromTextFunction = [&parameterInfo] (const String& text)
        { return static_cast<double> (parameterInfo.getValueForText (text)); };
        slider.textFromValueFunction = [&parameterInfo] (double value)
        { return parameterInfo.getText (parameterInfo.convertTo0to1 (value), 0); };
        slider.setDoubleClickReturnValue (true, parameterInfo.getDefaultValue());

        slider.setNormalisableRange (attachment.getRange());

        slider.addValueChangedFn (
            [this]()
            {
                if (attachment.getIgnoreCallbacks() || ModifierKeys::currentModifiers.isRightButtonDown())
                    return;
                attachment.setValueFromAttached (static_cast<float> (slider.getValue()));
            });
    }

    /** Destructor. */
    ~APVTSSliderAttachment() = default;

private:
    nlt::Slider&             slider;
    APVTSParameterAttachment attachment;

    void setValue (float newValue)
    {
        auto scopedIgnore = attachment.getScopedIgnoreCallbacks();
        slider.setValue (newValue, dontSendNotification);
        slider.valueChanged();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APVTSSliderAttachment)
};

} // namespace nlt
