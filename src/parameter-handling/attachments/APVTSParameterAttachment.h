/*
  ==============================================================================

    APVTSParameterAttachment.h
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include "ParameterAttachment.h"
#include "../APVTSCallback.h"
#include "../../utilities/instance-management/TimedActionInstanceManager.h"
#include "../../utilities/NLT_FWD.h"

namespace nlt
{

struct APVTSParameterAttachment : public ParameterGUIAttachment, public TimedActionInstance<APVTSParameterAttachment, 60, 1>
{
    template <typename Callback>
    APVTSParameterAttachment (RangedAudioParameter& _parameterInfo, Callback&& _callback)
        : ParameterGUIAttachment (_parameterInfo, NLT_FWD (_callback))
        , parameter (_parameterInfo)
        , value (parameter.getValue())
    {
        apvtsCallbacks.add (parameterInfo,
                            APVTSCallbacks::Type::gui,
                            [this] (float newValue)
                            {
                                value.store (parameter.convertTo0to1 (newValue));
                                updatedFromAutomation.store (true);
                            });
        //        value.store (parameter.getValue());
        //        updatedFromAutomation.store (true);
    }

    void handleGlobalTimedAction()
    {
        if (!ignoreCallbacks)
        {
            if (updatedFromAttachedElement.exchange (false))
                setValueAsCompleteGesture (parameter.convertFrom0to1 (value.load()));

            if (updatedFromAutomation.exchange (false))
                callback (parameter.convertFrom0to1 (value.load()));
        }
    }

    /** Triggers a full gesture message on the managed parameter.

    Call this in the listener callback of the UI control in response
            to a one-off change in the UI like a button-press.
                                                       */
    void setValueAsCompleteGesture (float newDenormalisedValue)
    {
        callIfParameterValueChanged (newDenormalisedValue,
                                     [this] (float f)
                                     {
                                         beginGesture();
                                         parameter.setValueNotifyingHost (parameter.convertTo0to1 (f));
                                         endGesture();
                                     });
    }

    /** Begins a gesture on the managed parameter.

        Call this when the UI is about to begin a continuous interaction,
        like when the mouse button is pressed on a slider.
    */
    void beginGesture() { parameter.beginChangeGesture(); }

    /** Updates the parameter value during a gesture.

        Call this during a continuous interaction, like a slider value
        changed callback.
    */
    void setValueAsPartOfGesture (float newDenormalisedValue)
    {
        callIfParameterValueChanged (newDenormalisedValue,
                                     [this] (float f)
                                     {
                                         parameter.beginChangeGesture();
                                         parameter.setValueNotifyingHost (f);
                                         parameter.endChangeGesture();
                                     });
    }

    /** Ends a gesture on the managed parameter.

        Call this when the UI has finished a continuous interaction,
        like when the mouse button is released on a slider.
    */
    void endGesture() { parameter.endChangeGesture(); }

    void setValueFromAttached (float newValue) override
    {
        value.store (parameter.convertTo0to1 (newValue));
        updatedFromAttachedElement.store (true);
    }

    void setValueFromAttached (const String& text) override
    {
        value.store (parameter.convertTo0to1 (getValueFromText (text)));
        updatedFromAttachedElement.store (true);
    }

private:
    RangedAudioParameter& parameter;
    std::atomic<float>    value;
    std::atomic<bool>     updatedFromAutomation { false };
    APVTSCallbacks        apvtsCallbacks;

    float normalise (float f) const { return parameter.convertTo0to1 (f); }

    template <typename Callback>
    void callIfParameterValueChanged (float newDenormalisedValue, Callback&& callback)
    {
        callback (newDenormalisedValue);
    }
};

} // namespace nlt
