#include "SliderMasked.h"

SliderMasked::SliderMasked()
{
    setNormalisableRange ({ 0.0, 1.0 });
    setSliderStyle (SliderStyle::LinearBarVertical);
    setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    setMouseCursor (MouseCursor::UpDownResizeCursor);
    setSliderSnapsToMousePosition (false);
}

void SliderMasked::paint (Graphics& g)
{
    g.drawImageWithin (bgImg, 0, 0, getWidth(), getHeight() - 1, RectanglePlacement::stretchToFit);
    auto value      = normalisedRange.convertTo0to1 (getValue());
    auto height     = static_cast<float> (getHeight() - 1);
    auto fillHeight = static_cast<float> (fillImg.getHeight());
    auto fillClip =
        fillImg.getClippedImage ({ 0,
                                   jmin (static_cast<int> (round (fillHeight - (value * fillHeight))), fillImg.getHeight() + 10),
                                   fillImg.getWidth(),
                                   jmin (static_cast<int> (round (value * fillHeight)), fillImg.getHeight() + 10) });
    g.drawImageWithin (fillClip,
                       0,
                       static_cast<int> (round (height - (value * height))),
                       getWidth(),
                       jmin (static_cast<int> (round (value * height)), getHeight() + 10),
                       RectanglePlacement::stretchToFit);
}

void SliderMasked::setImageBg (Image bg)
{
    bgImg = bg;
}

void SliderMasked::setImageFill (Image fill)
{
    fillImg = fill;
}

void SliderMasked::setParameter (RangedAudioParameter& parameter)
{
    attachment = std::make_unique<SliderParameterAttachment> (parameter, *this);
    attachment->sendInitialUpdate();
}
