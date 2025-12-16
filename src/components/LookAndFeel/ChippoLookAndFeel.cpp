/*
==============================================================================

    ChippoLookAndFeel.cpp

  ==============================================================================
  */

#include "ChippoLookAndFeel.h"

namespace ChippoLook
{

using namespace juce;

inline static Colour greyBG { 231, 232, 233 };
Look::Look()
{
    Typeface::Ptr beardays = juce::Typeface::createSystemTypefaceFor (BinaryData::Bear_Days_ttf, BinaryData::Bear_Days_ttfSize);
    auto          font     = Font (beardays).withExtraKerningFactor (0.5f);
    setDefaultSansSerifTypeface (font.getTypefacePtr());

    setColour (Label::textColourId, Colours::black);
    setColour (Label::textWhenEditingColourId, Colours::black);
    setColour (Label::backgroundWhenEditingColourId, greyBG);
    setColour (Label::textWhenEditingColourId, Colours::black);
    setColour (TextEditor::highlightedTextColourId, Colours::black);

    setColour (ToggleButton::tickDisabledColourId, Colours::black);
    setColour (ToggleButton::tickColourId, Colours::black);

    setColour (TextButton::buttonColourId, greyBG);
    setColour (TextButton::buttonOnColourId, greyBG.withMultipliedBrightness (0.3f));
    setColour (TextButton::textColourOffId, Colours::black);
    setColour (TextButton::textColourOnId, Colours::black);

    setColour (ComboBox::backgroundColourId, greyBG);
    setColour (ComboBox::outlineColourId, Colours::black);
    setColour (ComboBox::textColourId, Colours::black);

    setColour (PopupMenu::backgroundColourId, greyBG);
    setColour (PopupMenu::textColourId, Colours::black);
    setColour (PopupMenu::headerTextColourId, Colours::black);
    setColour (PopupMenu::highlightedTextColourId, Colours::black);
    setColour (PopupMenu::highlightedBackgroundColourId, greyBG.withMultipliedBrightness (0.3f));

    setColour (Slider::backgroundColourId, greyBG);
    setColour (Slider::textBoxBackgroundColourId, greyBG);
    setColour (Slider::rotarySliderFillColourId, greyBG);
    setColour (Slider::trackColourId, greyBG);
    setColour (Slider::textBoxTextColourId, Colours::black);
}

void Look::drawButtonBackground (Graphics&     g,
                                 Button&       button,
                                 const Colour& backgroundColour,
                                 bool          shouldDrawButtonAsHighlighted,
                                 bool          shouldDrawButtonAsDown)
{
    auto cornerSize = 6.0f;
    auto bounds     = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                          .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

    g.setColour (baseColour);

    auto flatOnLeft   = button.isConnectedOnLeft();
    auto flatOnRight  = button.isConnectedOnRight();
    auto flatOnTop    = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
    {
        Path path;
        path.addRoundedRectangle (bounds.getX(),
                                  bounds.getY(),
                                  bounds.getWidth(),
                                  bounds.getHeight(),
                                  cornerSize,
                                  cornerSize,
                                  !(flatOnLeft || flatOnTop),
                                  !(flatOnRight || flatOnTop),
                                  !(flatOnLeft || flatOnBottom),
                                  !(flatOnRight || flatOnBottom));

        g.fillPath (path);

        g.setColour (button.findColour (ComboBox::outlineColourId));
        g.strokePath (path, PathStrokeType (1.0f));
    }
    else
    {
        g.fillRect (bounds);

        g.setColour (button.findColour (ComboBox::outlineColourId));
        g.drawRect (bounds, 2.0f);
    }
}

void Look::drawLabel (juce::Graphics& g, juce::Label& label)
{
    using namespace juce;

    auto bounds = label.getLocalBounds();

    g.setColour (label.findColour (Label::backgroundColourId));
    g.fillRect (bounds);

    if (label.getBorderSize().getTop() > 0)
    {
        g.setColour (label.findColour (Label::outlineColourId));
        g.drawRect (bounds);
    }

    const String text     = label.getText();
    const Font   font     = label.getFont();
    const Colour colour   = label.findColour (Label::textColourId);
    const auto   area     = bounds.toFloat();
    const auto   just     = label.getJustificationType();
    auto         textArea = bounds; //getLabelBorderSize (label).subtractedFrom (label.getLocalBounds());

    g.setColour (colour);

    drawTextWithTracking (g, font, text, textArea.toFloat(), just, trackingPx);
}

void Look::drawToggleButton (Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto fontSize  = jmin (15.0f, (float) button.getHeight() * 0.75f);
    auto tickWidth = fontSize * 1.1f;

    auto buttonBounds = button.getLocalBounds().toFloat();
    auto length       = jmin (buttonBounds.getWidth(), buttonBounds.getHeight());
    {
        auto centre  = buttonBounds;
        buttonBounds = buttonBounds.withWidth (length).withHeight (length).withCentre (centre.getCentre());
    }
    drawTickBox (g,
                 button,
                 buttonBounds.getX(),
                 buttonBounds.getY(),
                 buttonBounds.getWidth(),
                 buttonBounds.getHeight(),
                 button.getToggleState(),
                 button.isEnabled(),
                 shouldDrawButtonAsHighlighted,
                 shouldDrawButtonAsDown);

    g.setColour (button.findColour (ToggleButton::textColourId));
    g.setFont (fontSize);

    if (!button.isEnabled())
        g.setOpacity (0.5f);

    g.drawFittedText (button.getButtonText(),
                      button.getLocalBounds().withTrimmedLeft (roundToInt (tickWidth) + 10).withTrimmedRight (2),
                      Justification::centredLeft,
                      10);
}

void Look::drawTickBox (Graphics&                   g,
                        Component&                  component,
                        float                       x,
                        float                       y,
                        float                       w,
                        float                       h,
                        const bool                  ticked,
                        [[maybe_unused]] const bool isEnabled,
                        [[maybe_unused]] const bool shouldDrawButtonAsHighlighted,
                        [[maybe_unused]] const bool shouldDrawButtonAsDown)
{
    Rectangle<float> tickBounds (x, y, w, h);

    //    g.setColour (component.findColour (ToggleButton::tickDisabledColourId));
    //    g.drawRoundedRectangle (tickBounds, 4.0f, 4.0f);
    g.setColour (greyBG);
    g.fillRect (tickBounds.toFloat());
    g.setColour (Colours::black);
    g.drawRect (tickBounds.toFloat(), 1.5f);

    if (ticked)
    {
        auto padding = 8.0f;
        Path p;
        p.startNewSubPath (padding, padding);
        p.lineTo (w - padding, h - padding);
        g.strokePath (p, PathStrokeType (5.0f));
        p.clear();
        p.startNewSubPath (w - padding, padding);
        p.lineTo (padding, h - padding);
        g.strokePath (p, PathStrokeType (5.0f));
        //        g.setColour (component.findColour (ToggleButton::tickColourId));
        //        auto tick = getTickShape (0.75f);
        //        g.fillPath (tick, tick.getTransformToScaleToFit (tickBounds.reduced (4, 5).toFloat(), false));
    }
}

void Look::drawButtonText (Graphics&   g,
                           TextButton& button,
                           bool /*shouldDrawButtonAsHighlighted*/,
                           bool /*shouldDrawButtonAsDown*/)
{
    Font font (getTextButtonFont (button, button.getHeight()).withExtraKerningFactor (0.05f));
    g.setFont (font);
    g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId : TextButton::textColourOffId)
                     .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    const int yIndent    = jmin (4, button.proportionOfHeight (0.3f));
    const int cornerSize = jmin (button.getHeight(), button.getWidth()) / 2;

    const int fontHeight  = roundToInt (font.getHeight() * 0.6f);
    const int leftIndent  = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    const int textWidth   = button.getWidth() - leftIndent - rightIndent;

    if (textWidth > 0)
        g.drawFittedText (button.getButtonText(),
                          leftIndent,
                          yIndent,
                          textWidth,
                          button.getHeight() - yIndent * 2,
                          Justification::centred,
                          2);
}

void Look::drawComboBox (Graphics& g, int width, int height, bool, int, int, int, int, ComboBox& box)
{
    //    auto           cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRect (boxBounds.toFloat());

    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRect (boxBounds.toFloat(), 1.5f); // .reduced (0.5f, 0.5f)

    //    Rectangle<int> arrowZone (width - 30, 0, 20, height);
    //    Path           path;
    //    path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
    //    path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
    //    path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);
}

Font Look::getComboBoxFont (ComboBox& box)
{
    return { (float) box.getHeight() * 0.60f };
    //    return { jmin (16.0f, (float) box.getHeight() * 1.5f) };
}

void Look::positionComboBoxText (ComboBox& box, Label& label)
{
    label.setBounds (0, 0, box.getWidth(), box.getHeight());

    label.setFont (getComboBoxFont (box));

    label.setMouseCursor (MouseCursor::PointingHandCursor);
}

void Look::drawLinearSlider (Graphics&                 g,
                             int                       x,
                             int                       y,
                             int                       width,
                             int                       height,
                             float                     sliderPos,
                             float                     minSliderPos,
                             float                     maxSliderPos,
                             const Slider::SliderStyle style,
                             Slider&                   slider)
{
    g.setColour (greyBG);
    g.fillRect (x, y, width, height);
    LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    g.setColour (slider.findColour (Slider::textBoxOutlineColourId));
    g.drawRect (slider.getLocalBounds().toFloat(), 2.0f);
}

void Look::drawBubble (Graphics& g, BubbleComponent& comp, const Point<float>& tip, const Rectangle<float>& body)
{
    Path p;
    p.addBubble (body.reduced (0.5f),
                 body.getUnion (Rectangle<float> (tip.x, tip.y, 1.0f, 1.0f)),
                 tip,
                 5.0f,
                 jmin (15.0f, body.getWidth() * 0.2f, body.getHeight() * 0.2f));

    g.setColour (comp.findColour (BubbleComponent::backgroundColourId));

    g.fillRect (body);
    //        g.fillPath (p);

    g.setColour (comp.findColour (BubbleComponent::outlineColourId));
    g.drawRect (body);
    //        g.strokePath (p, PathStrokeType (1.0f));
}

Label* Look::createSliderTextBox (Slider& slider)
{
    auto* l = LookAndFeel_V4::createSliderTextBox (slider);
    l->setFont ({ 18.0f });
    l->onEditorShow = [l]()
    {
        auto editor = l->getCurrentTextEditor();
        editor->setJustification (Justification::centred);
    };

    return l;
}

SequencerToggleLook::SequencerToggleLook()
{
    setColour (ToggleButton::tickDisabledColourId, Colours::black);
    setColour (ToggleButton::tickColourId, Colours::black);
}

void SequencerToggleLook::drawToggleButton (Graphics&     g,
                                            ToggleButton& button,
                                            bool          shouldDrawButtonAsHighlighted,
                                            bool          shouldDrawButtonAsDown)
{
    auto fontSize  = jmin (15.0f, (float) button.getHeight() * 0.75f);
    auto tickWidth = fontSize * 1.1f;

    auto buttonBounds = button.getLocalBounds().toFloat();
    auto length       = jmin (buttonBounds.getWidth(), buttonBounds.getHeight());
    {
        auto centre  = buttonBounds; //.withHeight (length)
        buttonBounds = buttonBounds.withWidth (length).withCentre (centre.getCentre());
    }
    drawTickBox (g,
                 button,
                 buttonBounds.getX(),
                 buttonBounds.getY(),
                 buttonBounds.getWidth(),
                 buttonBounds.getHeight(),
                 button.getToggleState(),
                 button.isEnabled(),
                 shouldDrawButtonAsHighlighted,
                 shouldDrawButtonAsDown);
}

void SequencerToggleLook::drawTickBox (Graphics&             g,
                                       Component&            component,
                                       float                 x,
                                       float                 y,
                                       float                 w,
                                       float                 h,
                                       bool                  ticked,
                                       [[maybe_unused]] bool isEnabled,
                                       [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
                                       [[maybe_unused]] bool shouldDrawButtonAsDown)
{
    juce::Rectangle<float> tickBounds (x, y, w, h);

    g.setColour (component.findColour (juce::ToggleButton::textColourId));
    g.fillRoundedRectangle (tickBounds, 4.0f);
    g.setColour (component.findColour (juce::ToggleButton::tickDisabledColourId));
    g.drawRoundedRectangle (tickBounds, 4.0f, 2.0f);

    g.setColour (component.findColour (juce::ToggleButton::tickColourId));
    if (ticked)
        g.fillRoundedRectangle (tickBounds.reduced (2.0f), 4.0f);
}

} // namespace ChippoLook
