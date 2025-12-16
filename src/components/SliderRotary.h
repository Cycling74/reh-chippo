#pragma once
#include "../parameter-handling/Slider.h"
#include "RNBO.h"

struct SliderRotary : public nlt::Slider
{
    SliderRotary();

    void paint (Graphics& g) override;

    void addImage (const Image& newImg);

    void clearImages();

    void setParameter (RangedAudioParameter& parameter);

private:
    float                                      prevNormalisedValue { 0.0f };
    std::vector<Image>                         images;
    std::unique_ptr<SliderParameterAttachment> attachment;
    size_t                                     currentImageIndex { 0 };

    size_t getImageIndex();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderRotary)
};
