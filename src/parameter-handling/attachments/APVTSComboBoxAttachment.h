/*
  ==============================================================================

    APVTSComboBoxAttachment.h
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include "APVTSParameterAttachment.h"

namespace nlt
{

struct APVTSComboBoxAttachment : private ComboBox::Listener
{
    APVTSComboBoxAttachment (RangedAudioParameter& parameterInfo, ComboBox& c)
        : comboBox (c)
        , attachment (parameterInfo, [this] (float f) { setValue (f); })
        , range (attachment.getRange())
    {
        if (c.getNumItems() == 0)
        {
            for (int val = range.start, i = 1; val <= range.end; ++i, ++val)
            {
                c.addItem (parameterInfo.getText (static_cast<float> (val) / static_cast<float> (range.end), 999), i);
            }

            c.setJustificationType (Justification::centred);
        }

        comboBox.addListener (this);
    }

    /** Destructor. */
    ~APVTSComboBoxAttachment() override { comboBox.removeListener (this); }

private:
    ComboBox&                 comboBox;
    APVTSParameterAttachment  attachment;
    NormalisableRange<double> range;

    void setValue (float newValue)
    {
        //        auto       range     = attachment.getRange();
        //        const auto normValue = range.convertTo0to1 (newValue);
        //        const auto index = roundToInt (normValue * (float) (comboBox.getNumItems() - 1));
        //        const auto index = newValue;

        const auto normValue = range.convertTo0to1 (newValue);
        const auto index     = roundToInt (normValue * (float) (comboBox.getNumItems() - 1));

        if (index == comboBox.getSelectedItemIndex())
            return;

        auto scopedIgnore = attachment.getScopedIgnoreCallbacks();
        comboBox.setSelectedItemIndex (index, sendNotificationAsync);
    }

    void comboBoxChanged (ComboBox*) override
    {
        if (attachment.getIgnoreCallbacks())
            return;

        const auto numItems = comboBox.getNumItems();
        const auto selected = (float) comboBox.getSelectedItemIndex();
        const auto newValue = numItems > 1 ? selected / (float) (numItems - 1) : 0.0f;
        //        auto val = range.convertTo0to1 (selected);
        attachment.setValueFromAttached (range.convertFrom0to1 (newValue));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APVTSComboBoxAttachment)
};

} // namespace nlt
