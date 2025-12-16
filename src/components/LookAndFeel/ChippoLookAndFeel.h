/*
==============================================================================

    ChippoLookAndFeel.h

  ==============================================================================
  */
#include <JuceHeader.h>

#define SET_DEFAULT_LOOK_AND_FEEL(lookTypeToSet) SharedResourcePointer<DefaultLookAndFeel<lookTypeToSet>> defaultLook;

template <typename LookType>
struct DefaultLookAndFeel
{
    DefaultLookAndFeel() { LookAndFeel::setDefaultLookAndFeel (&look); }
    ~DefaultLookAndFeel() { LookAndFeel::setDefaultLookAndFeel (nullptr); }

    LookType look;

    JUCE_LEAK_DETECTOR (DefaultLookAndFeel)
};

static constexpr float fontTracking = 1.3f;

static inline void drawTextWithTracking (Graphics&        g,
                                         const Font&      font,
                                         const String&    text,
                                         Rectangle<float> area,
                                         Justification    justification,
                                         float            trackingPx)
{
    GlyphArrangement ga;
    float            baselineY = area.getY() + font.getAscent();

    ga.addLineOfText (font, text, 0.0f, baselineY);

    for (int i = 0; i < ga.getNumGlyphs(); ++i)
    {
        ga.getGlyph (i).moveBy (i * trackingPx, 0.0f);
    }

    auto bounds = ga.getBoundingBox (0, ga.getNumGlyphs(), true);

    float xOffset = 0.0f;

    if (justification.testFlags (Justification::horizontallyCentred))
        xOffset = area.getX() + (area.getWidth() - bounds.getWidth()) * 0.5f - bounds.getX();
    else if (justification.testFlags (Justification::right))
        xOffset = area.getRight() - bounds.getRight();
    else
        xOffset = area.getX() - bounds.getX();

    float yOffset = area.getY() + (area.getHeight() - bounds.getHeight()) * 0.5f - bounds.getY();

    ga.moveRangeOfGlyphs (0, ga.getNumGlyphs(), xOffset, yOffset);

    ga.draw (g);
}

/**
 * this will rotate your text -90º using the top left corner of the textArea as the pivot point
 */
static inline void drawRotatedText (Graphics& g, const String& text, Rectangle<float> textArea, float degrees = -90.0f)
{
    AffineTransform rotation = AffineTransform::rotation (degreesToRadians (degrees), textArea.getX(), textArea.getY());

    g.saveState();
    g.addTransform (rotation);
    drawTextWithTracking (g, g.getCurrentFont(), text, textArea, Justification::centred, fontTracking);
    g.restoreState();
}

namespace ChippoLook
{

using namespace juce;

struct Look : public LookAndFeel_V4
{
    Look();

    static constexpr float trackingPx { 1.3f };

    void drawButtonBackground (Graphics&     g,
                               Button&       button,
                               const Colour& backgroundColour,
                               bool          shouldDrawButtonAsHighlighted,
                               bool          shouldDrawButtonAsDown) override;

    void drawToggleButton (Graphics&     g,
                           ToggleButton& button,
                           bool          shouldDrawButtonAsHighlighted,
                           bool          shouldDrawButtonAsDown) override;

    void drawTickBox (Graphics&             g,
                      Component&            component,
                      float                 x,
                      float                 y,
                      float                 w,
                      float                 h,
                      bool                  ticked,
                      [[maybe_unused]] bool isEnabled,
                      [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
                      [[maybe_unused]] bool shouldDrawButtonAsDown) override;

    void drawComboBox (Graphics& g, int width, int height, bool, int, int, int, int, ComboBox& box) override;

    void positionComboBoxText (ComboBox& box, Label& label) override;

    Font getComboBoxFont (ComboBox& box) override;

    void drawBubble (Graphics& g, BubbleComponent& comp, const Point<float>& tip, const Rectangle<float>& body) override;

    void drawLabel (juce::Graphics& g, juce::Label& label) override;

    Label* createSliderTextBox (Slider& slider) override;

    void drawLinearSlider (Graphics&,
                           int   x,
                           int   y,
                           int   width,
                           int   height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           Slider::SliderStyle,
                           Slider&) override;

    void drawButtonText (Graphics&, TextButton&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

struct LinearBarVerticalLook : public Look
{
};

struct SequencerToggleLook : public juce::LookAndFeel_V4
{
    SequencerToggleLook();

    void drawToggleButton (Graphics&     g,
                           ToggleButton& button,
                           bool          shouldDrawButtonAsHighlighted,
                           bool          shouldDrawButtonAsDown) override;

    void drawTickBox (Graphics&             g,
                      Component&            component,
                      float                 x,
                      float                 y,
                      float                 w,
                      float                 h,
                      bool                  ticked,
                      [[maybe_unused]] bool isEnabled,
                      [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
                      [[maybe_unused]] bool shouldDrawButtonAsDown) override;
};

} // namespace ChippoLook
