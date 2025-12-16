/*
  ==============================================================================

    APVTSLabelAttachment.h
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include "APVTSParameterAttachment.h"

namespace nlt
{

struct APVTSLabelAttachment
{
    /** Creates a connection between a plug-in parameter and a Label.
    */
    APVTSLabelAttachment (RangedAudioParameter& parameterInfo, Label& l)
        : label (l)
        , attachment (parameterInfo, [this] (float f) { setValue (f); })
    {
    }

    /** Destructor. */
    ~APVTSLabelAttachment() = default;

    void setValueFromText (const String& text)
    {
        if (!attachment.getIgnoreCallbacks())
        {
            auto range = attachment.getRange();
            attachment.setValueFromAttached (range.convertFrom0to1 (attachment.getValueFromText (text)));
        }
    }

private:
    Label&                   label;
    APVTSParameterAttachment attachment;

    void setValue (float newValue)
    {
        auto scoped = attachment.getScopedIgnoreCallbacks();
        label.setText (attachment.getTextFromValue (newValue), dontSendNotification);
        label.onTextChange();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APVTSLabelAttachment)
};

} // namespace nlt
