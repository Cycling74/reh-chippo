#pragma once
#include <JuceHeader.h>

struct AboutPanel : public Component
{
    AboutPanel() { setMouseCursor (MouseCursor::PointingHandCursor); }

    void mouseDown (const MouseEvent& event) override { setVisible (false); }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black.withAlpha (0.5f));
        auto textBox = Rectangle<float> (0, 0, 450, 225).withCentre (getBounds().getCentre().toFloat());
        g.setColour (Colours::white);
        g.fillRect (textBox);
        //        g.setColour (Colour (231, 232, 233));
        g.setColour (Colour (232, 203, 125));
        g.drawRoundedRectangle (textBox, 5, 5);

        g.setColour (Colours::black);
        g.setFont (Font (12.0f).withExtraKerningFactor (0.05f));
        g.drawFittedText (ProjectInfo::versionString,
                          textBox.toNearestInt().removeFromBottom (16).removeFromRight (50).translated (-5, -3),
                          Justification::right,
                          1);

        textBox.reduce (20, 20);
        g.setFont (Font (18.0f).withExtraKerningFactor (0.05f));
        g.drawFittedText (CharPointer_UTF8 ("Chippo by Emily Hopkins\n"
                                            "with help from Cycling '74\n"
                                            "(c) 2025 REH Creative LLC\n\n"
                                            "Concept: Emily Hopkins, Russ Fro\n"
                                            "Programming: Mike Rugnetta, Dave Sanchez\n"
                                            "Artwork: Emily, Russ\n"
                                            "UI: Dave, Mike, Emily, Russ\n\n"
                                            "Made in RNBO by Cycling ‘74, and with JUCE"),
                          textBox.toNearestInt(),
                          Justification::left,
                          50);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutPanel)
};
