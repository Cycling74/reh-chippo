/*
  ==============================================================================

    APVTSToggleAttachment.h
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include "APVTSParameterAttachment.h"

namespace nlt
{

struct APVTSToggleAttachment : private Button::Listener
{
    /** Creates a connection between a plug-in parameter and a Button.

    @param parameter     The parameter to use
        @param button        The Button to use
        @param undoManager   An optional UndoManager
            */
    APVTSToggleAttachment (RangedAudioParameter& parameterInfo, Button& b)
        : button (b)
        , attachment (parameterInfo, [this] (float f) { setValue (f); })
    {
        button.addListener (this);
    }

    /** Destructor. */
    ~APVTSToggleAttachment() override { button.removeListener (this); }

private:
    Button&                  button;
    APVTSParameterAttachment attachment;

    void setValue (float newValue)
    {
        auto scoped = attachment.getScopedIgnoreCallbacks();
        button.setToggleState (newValue > 0.5f, sendNotificationSync);
    }
    void buttonClicked (Button*) override
    {
        if (attachment.getIgnoreCallbacks())
            return;

        attachment.setValueFromAttached (static_cast<float> (button.getToggleState() ? 1.0f : 0.0f));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APVTSToggleAttachment)
};

} // namespace nlt
