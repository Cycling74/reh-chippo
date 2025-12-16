/*
 ==============================================================================

    APVTSControl.h
    Author:  Dave Sanchez

 ==============================================================================
 */

#pragma once
#include "attachments/APVTSSliderAttachment.h"
#include "attachments/APVTSToggleAttachment.h"
#include "attachments/APVTSComboBoxAttachment.h"

namespace nlt
{

using namespace juce;

/**
 * @see APVTSControls
 * @tparam SliderButtonOrBoxType
 */
template <class SliderButtonOrBoxType>
struct APVTSControl : public SliderButtonOrBoxType
{
    // sets to the appropriate APVTS attachment type
    using AVT        = AudioProcessorValueTreeState;
    using Attachment = typename std::conditional<
        std::is_base_of<Button, SliderButtonOrBoxType>::value,
        APVTSToggleAttachment,
        typename std::conditional<std::is_base_of<Slider, SliderButtonOrBoxType>::value,
                                  APVTSSliderAttachment,
                                  typename std::conditional<std::is_base_of<ComboBox, SliderButtonOrBoxType>::value,
                                                            APVTSComboBoxAttachment,
                                                            void>::type>::type>::type;

public:
    explicit APVTSControl (RangedAudioParameter* _parameter)
        : parameter (_parameter)
        , parameterName (_parameter->getParameterID())
    {
        // check that the parameter you're attaching to exists
        setParameter (parameter);

        if constexpr (std::is_base_of<Button, SliderButtonOrBoxType>::value
                      || std::is_base_of<ComboBox, SliderButtonOrBoxType>::value)
            this->setMouseCursor (MouseCursor::PointingHandCursor);

        if constexpr (std::is_base_of<nlt::Slider, SliderButtonOrBoxType>::value)
            this->normalisedRange = parameter->getNormalisableRange();
    }

    ~APVTSControl() = default;

    void setParameter (RangedAudioParameter* _parameterInfo)
    {
        attachment.reset();
        parameter     = _parameterInfo;
        parameterName = parameter->getParameterID();

        this->setName (parameterName);

        if constexpr (std::is_base_of<nlt::Slider, SliderButtonOrBoxType>::value)
            this->resetValueChangedFns();

        attachment.reset (new Attachment (*parameter, *this));
    }

    String getParamDisplayName()
    {
        if (parameter != nullptr)
            return parameter->getName (100);
        else
            return {};
    }

    String getTextForParameterValue()
    {
        if (parameter != nullptr)
            return parameter->getText (parameter->getValue(), 999);
        else
            return {};
    }

    const RangedAudioParameter& getParamInfo() const { return *parameter; }

private:
    std::unique_ptr<Attachment> attachment;
    RangedAudioParameter*       parameter;
    String                      parameterName;

    static_assert (std::is_base_of<Slider, SliderButtonOrBoxType>::value || std::is_base_of<Button, SliderButtonOrBoxType>::value
                       || std::is_base_of<ComboBox, SliderButtonOrBoxType>::value,
                   "only built for sliders, buttons, or comboboxes");

    JUCE_LEAK_DETECTOR (APVTSControl)
};

} // namespace nlt
