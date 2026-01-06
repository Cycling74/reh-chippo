#pragma once
#include <JuceHeader.h>
//#include "LookAndFeel/ChippoLookAndFeel.h"

struct EditorBackground : public Component
{
    EditorBackground()
    {
        setRepaintsOnMouseActivity (false);
        setBufferedToImage (true);
        setOpaque (true);
        bannerImg.setImage (ImageCache::getFromMemory (BinaryData::Banner_png, BinaryData::Banner_pngSize));
        bannerImg.setOpaque (true);
        addAndMakeVisible (bannerImg);
    }

    void resized() override { bannerImg.setBounds (0, 0, getWidth(), 120); }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);

        g.setColour (Colours::black);
        g.setFont (Font (35.0f));

        drawRotatedText (g, "Melody", { 21, 335, 118, 26 });
        drawRotatedText (g, "Bass", { 21, 519, 74, 26 });
        drawRotatedText (g, "Drums", { 21, 729, 94, 26 });
        g.setFont (Font (29.0f));
        drawRotatedText (g, "8", { 980, 267, 34, 28 }, 90.0f);
        //        { toggles[Toggles::generateMelodyAlways]->getRight() + 6,     956
        //                toggles[Toggles::generateMelodyAlways]->getY(),       270
        //                24,
        //                29 };

        auto textHeight  = 28;
        auto writeLabels = [textHeight, &g] (int yPos)
        {
            g.setFont (18.0f);
            drawTextWithTracking (g, Font(), "OCT", { 150.0f, yPos + 5.0f, 38.0f, 18.0f }, Justification::centred, 1.0f);
            g.setFont (float (textHeight));
            drawTextWithTracking (g,
                                  Font ((float) textHeight),
                                  "VOL",
                                  { 82.0f, (float) yPos, 50.0f, (float) textHeight },
                                  Justification::centred,
                                  fontTracking);
            auto largerText = textHeight + 6;
            g.setFont (static_cast<float> (largerText));
            g.drawFittedText ("A", 377, yPos, 71, largerText, Justification::centred, 1);
            g.drawFittedText ("D", 513, yPos, 71, largerText, Justification::centred, 1);
            g.drawFittedText ("S", 650, yPos, 71, largerText, Justification::centred, 1);
            g.drawFittedText ("R", 790, yPos, 71, largerText, Justification::centred, 1);
        };
        writeLabels (345);
        writeLabels (550);
        Font font { static_cast<float> (textHeight) };
        drawTextWithTracking (g, font, "SHAPE", { 215, 345, 71, (float) textHeight }, Justification::centred, fontTracking);
        drawTextWithTracking (g, font, "GLIDE", { 217, 550, 68, (float) textHeight }, Justification::centred, fontTracking);
        //        g.setFont (Font(textHeight).withExtraKerningFactor (0.05f));
        //        g.drawText ("KICK", Rectangle<int>{ 74, 744, 68, 35 }, Justification::centred);
        //        g.drawText ("SNARE", Rectangle<int>{ 318, 744, 125, 35 }, Justification::centred);
        //        g.drawText ("HAT", Rectangle<int>{ 569, 744, 68, 35 }, Justification::centred);
        //        g.drawText ("REVERB", Rectangle<int>{ 958, 744, 125, 35 }, Justification::centred);

        drawTextWithTracking (g, font, "KICK", { 74, 744, 68, (float) textHeight }, Justification::centred, fontTracking);
        drawTextWithTracking (g, font, "SNARE", { 318, 744, 68, (float) textHeight }, Justification::centred, fontTracking);
        drawTextWithTracking (g, font, "HAT", { 569, 744, 68, (float) textHeight }, Justification::centred, fontTracking);
        drawTextWithTracking (g, font, "REVERB", { 958, 744, 68, (float) textHeight }, Justification::centred, fontTracking);

        auto space = 5;
        g.setFont (Font (27.0f).withExtraKerningFactor (0.05f));
        g.drawText ("ROOT", Rectangle<int> { 305 + space, 140, 150, 35 }, Justification::left);
        //        drawTextWithTracking (g, font, "ROOT", { 297, 140, 150, 35 }, Justification::left, fontTracking);
        g.drawText ("SCALE", Rectangle<int> { 566 + space, 140, 150, 35 }, Justification::left);
        //        drawTextWithTracking (g, font, "SCALE", { 531, 140, 150, 35 }, Justification::left, fontTracking);
        g.drawText ("STEPS", Rectangle<int> { 751 + space, 140, 150, 35 }, Justification::left);
        g.drawText ("DENSITY", Rectangle<int> { 946 + space, 140, 150, 35 }, Justification::left);

        auto greyBG = Colour (231, 232, 233);
        g.setColour (greyBG);
        g.fillRect (Rectangle<int> { 0, 805, getWidth(), getHeight() - 805 }); // sequencer grey BG

        g.setFont (Font (32.0f).withExtraKerningFactor (0.05f));
        g.setColour (Colours::black);
        g.drawText ("SEQUENCER", Rectangle<int> { 21, 810, 175, 30 }, Justification::topLeft);
    }

private:
    ImageComponent bannerImg;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorBackground)
};
