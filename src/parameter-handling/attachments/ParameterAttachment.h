/*
  ==============================================================================

    ParameterGUIAttachment.h
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../../utilities/NLT_FWD.h"

namespace nlt
{

struct ParameterGUIAttachment
{
    template <typename Callback>
    ParameterGUIAttachment (RangedAudioParameter& _parameterInfo, Callback&& _callback)
        : parameterInfo (_parameterInfo)
        , callback (NLT_FWD (_callback))
    {
        textFromValueFn = [this] (float val, int) { return parameterInfo.getText (parameterInfo.getValue(), 999); };
        valueFromTextFn = [this] (const String& text) { return parameterInfo.getValueForText (text); };
    }

    virtual ~ParameterGUIAttachment() = default;

    virtual void setValueFromAttached (float newValue)         = 0;
    virtual void setValueFromAttached (const String& newValue) = 0;

    String getText() { return getTextFromValue (parameterInfo.getNormalisableRange().convertFrom0to1 (normalisedValue.load())); }

    float getValueFromText (const String& text) { return valueFromTextFn (text); }

    String getTextFromValue (double value) { return textFromValueFn (value, 9999); }

    NormalisableRange<double> getRange()
    {
        auto floatRange              = parameterInfo.getNormalisableRange();
        auto convertFrom0To1Function = [floatRange] (double currentRangeStart, double currentRangeEnd, double normValue) mutable
        {
            floatRange.start = (float) currentRangeStart;
            floatRange.end   = (float) currentRangeEnd;
            return (double) floatRange.convertFrom0to1 ((float) normValue);
        };

        auto convertTo0To1Function = [floatRange] (double currentRangeStart, double currentRangeEnd, double mappedValue) mutable
        {
            floatRange.start = (float) currentRangeStart;
            floatRange.end   = (float) currentRangeEnd;
            return (double) floatRange.convertTo0to1 ((float) mappedValue);
        };

        auto snapToLegalValueFunction =
            [floatRange] (double currentRangeStart, double currentRangeEnd, double mappedValue) mutable
        {
            floatRange.start = (float) currentRangeStart;
            floatRange.end   = (float) currentRangeEnd;
            return (double) floatRange.snapToLegalValue ((float) mappedValue);
        };

        NormalisableRange<double> newRange { (double) floatRange.start,
                                             (double) floatRange.end,
                                             std::move (convertFrom0To1Function),
                                             std::move (convertTo0To1Function),
                                             std::move (snapToLegalValueFunction) };
        newRange.interval      = floatRange.interval;
        newRange.skew          = floatRange.skew;
        newRange.symmetricSkew = floatRange.symmetricSkew;
        return newRange;
    }

    String getParameterName() { return parameterInfo.getName (999); }

    std::unique_ptr<ScopedValueSetter<bool>> getScopedIgnoreCallbacks()
    {
        return std::make_unique<ScopedValueSetter<bool>> (ignoreCallbacks, true);
    }

    bool getIgnoreCallbacks() const { return ignoreCallbacks; }

protected:
    RangedAudioParameter&                parameterInfo;
    std::function<void (float)>          callback { nullptr };
    std::atomic<float>                   normalisedValue { 0.0f };
    std::atomic<bool>                    updatedFromAttachedElement { false };
    bool                                 ignoreCallbacks { false };
    std::function<String (float, int)>   textFromValueFn { nullptr };
    std::function<float (const String&)> valueFromTextFn { nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterGUIAttachment)
};

} // namespace nlt
