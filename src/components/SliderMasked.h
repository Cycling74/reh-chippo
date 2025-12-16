#pragma once
#include "../parameter-handling/Slider.h"
#include "RNBO.h"

struct SliderMasked : public nlt::Slider
{
    SliderMasked();

    void paint (Graphics& g) override;

    void setImageBg (Image bg);
    void setImageFill (Image fill);

    void setParameter (RangedAudioParameter& parameter);

private:
    Image                                      bgImg;
    Image                                      fillImg;
    std::unique_ptr<SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderMasked)
};
