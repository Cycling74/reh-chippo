/*
  ==============================================================================

    ValueTreeToggleButton.h
    Author:  David Sanchez

  ==============================================================================
*/
#pragma once
#include "ValueTreeCallback.h"

namespace nlt
{

using namespace juce;

struct ValueTreeToggleButton : public juce::ImageButton
{
    ValueTreeToggleButton (juce::ValueTree tree, const juce::Identifier& _property)
        : valueTree (tree)
        , property (_property)
    {
        setClickingTogglesState (true);
        vtCallbacks.add (valueTree, property, [this] (bool state) { setToggleState (state, juce::sendNotification); });
        onClick = [this]() { valueTree.setProperty (property, getToggleState(), nullptr); };
        setMouseCursor (juce::MouseCursor::PointingHandCursor);
    }

    ~ValueTreeToggleButton() override { setLookAndFeel (nullptr); }

private:
    ValueTree          valueTree;
    juce::Identifier   property;
    ValueTreeCallbacks vtCallbacks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeToggleButton)
};

} // namespace nlt
