/*
  ==============================================================================

    FileChooserHolder.h
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include "NLT_FWD.h"

using namespace juce;
struct FileChooserHolder
{
    template <typename Fn>
    void launchFileChooser (const juce::String& title,
                            juce::File          path,
                            const juce::String& filePatternsAllowed,
                            int                 flags,
                            Fn&&                callback)
    {
        fileChooser = std::make_unique<juce::FileChooser> (title, path, filePatternsAllowed);
        fileChooser->launchAsync (flags, NLT_FWD (callback));
    }

private:
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_LEAK_DETECTOR (FileChooserHolder)
};
