#include "SliderRotary.h"

SliderRotary::SliderRotary()
{
    setNormalisableRange ({ 0.0, 1.0 });
    setSliderStyle (SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    setMouseCursor (MouseCursor::UpDownResizeCursor);
    setSliderSnapsToMousePosition (false);
}

void SliderRotary::paint (Graphics& g)
{
    if (images.empty())
    {
        g.setColour (Colours::red);
        g.drawFittedText ("NO IMAGE IN SLIDER", getLocalBounds(), Justification::centred, 10);
        return;
    }

    auto& img = images[getImageIndex()];

    auto travelRotation = juce::degreesToRadians (normalisedRange.convertTo0to1 (static_cast<float> (getValue())) * 270.0f);

    auto angle = travelRotation;

    auto imgW = static_cast<float> (img.getWidth());
    auto imgH = static_cast<float> (img.getHeight());

    auto compW = static_cast<float> (getWidth());
    auto compH = static_cast<float> (getHeight());

    auto scaleFactor = juce::jmin (compW / imgW, compH / imgH);

    auto scaledW = imgW * scaleFactor;
    auto scaledH = imgH * scaleFactor;

    auto drawX = (compW - scaledW) * 0.5f;
    auto drawY = (compH - scaledH) * 0.5f;

    auto pivotX = drawX + scaledW * 0.5f;
    auto pivotY = drawY + scaledH * 0.5f;

    juce::AffineTransform t =
        juce::AffineTransform::scale (scaleFactor).translated (drawX, drawY).rotated (angle, pivotX, pivotY);

    g.drawImageTransformed (img, t, false);
}

void SliderRotary::addImage (const Image& newImg)
{
    images.push_back (newImg);
    auto size = (int) images.size();
    if (size > 1)
        currentImageIndex = (size_t) Random().nextInt (size);
}

void SliderRotary::setParameter (RangedAudioParameter& parameter)
{
    attachment = std::make_unique<SliderParameterAttachment> (parameter, *this);
    attachment->sendInitialUpdate();
}

void SliderRotary::clearImages()
{
    images.clear();
    repaint();
}

// this was a goofy animated kind of thing but it's nbd
size_t SliderRotary::getImageIndex()
{
    if (images.size() == 1)
        return 0;

    auto currentNormalisedValue = normalisedRange.convertTo0to1 (static_cast<float> (getValue()));
    // splits up the knob into 5 trigger points.
    // the 0.99f is just so the max value doesn't trigger a new image bc that looks janky
    auto numTriggerPoints = 4.99f;
    auto truncVal         = static_cast<int> (currentNormalisedValue * numTriggerPoints);
    auto prevTruncVal     = static_cast<int> (prevNormalisedValue * numTriggerPoints);

    if (truncVal != prevTruncVal)
    {
        auto size         = images.size();
        auto indexOffset  = size == 2 ? 1 : (size_t) (Random().nextInt (jmax (1, (int) size - 2))) + 1;
        currentImageIndex = (currentImageIndex + indexOffset) % size;
    }
    prevNormalisedValue = currentNormalisedValue;

    return currentImageIndex;
}
