/*
  ==============================================================================

    VoidActions
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "NLT_FWD.h"

namespace nlt
{
using namespace juce;

/**
 * For holding a bunch of void functions;
 */
class VoidActions
{
public:
    VoidActions() { actions.clear(); }
    ~VoidActions() = default;

    template <typename Fn>
    void addAction (Fn&& fn)
    {
        actions.push_back (NLT_FWD (fn));
    }

    void clearActions() { actions.clear(); }

    void triggerActions()
    {
        for (auto& a: actions)
            a();
    }

private:
    std::vector<std::function<void()>> actions;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoidActions)
};

} // namespace nlt
