/*
  ==============================================================================

    ChangeListenerActions.h
    Created: 15 Dec 2022 5:01:22pm
    Author:  David Sanchez

  ==============================================================================
*/
#pragma once
#include "ChangeListenerRAII.h"
namespace nlt
{

/**
 A single change listener action. You should probably use ChangeListenerActions instead.
 */
struct SingleChangeListenerAction final : public ChangeListenerRAII
{
    SingleChangeListenerAction() = default;

    template <typename Fn>
    SingleChangeListenerAction (juce::ChangeBroadcaster& broadcaster, Fn&& newAction)
        : action (NLT_FWD (newAction))
    {
        addAsChangeListenerTo (broadcaster);
    }

    void changeListenerCallback (ChangeBroadcaster* source) override { action (source); }

    template <typename Fn>
    void setChangeListenerAction (juce::ChangeBroadcaster& broadcaster, Fn&& newAction) &
    {
        removeFromAllBroadcasters();
        action = NLT_FWD (newAction);
        addAsChangeListenerTo (broadcaster);
    }

private:
    std::function<void (juce::ChangeBroadcaster*)> action { [] (juce::ChangeBroadcaster*) {} };

    JUCE_LEAK_DETECTOR (SingleChangeListenerAction)
};

/**
 Holds an arbitrary amount of change listener actions
 */
struct ChangeListenerActions final
{
    template <typename Fn>
    void add (juce::ChangeBroadcaster& broadcaster, Fn&& newAction) &
    {
        actions.add (new SingleChangeListenerAction (broadcaster, NLT_FWD (newAction)));
    }

    bool removeActionsForSourceBroadcaster (juce::ChangeBroadcaster& broadcaster) &
    {
        std::vector<SingleChangeListenerAction*> actionsToRemove;
        for (auto* a: actions)
            if (a->removeAsChangeListenerFrom (broadcaster))
                actionsToRemove.emplace_back (a);

        bool hadActionsForSource = !actionsToRemove.empty();

        for (auto& a: actionsToRemove)
            actions.removeObject (a);

        return hadActionsForSource;
    }

    void clear() & { actions.clear(); }

private:
    OwnedArray<SingleChangeListenerAction> actions;

    JUCE_LEAK_DETECTOR (ChangeListenerActions)
};

} // namespace nlt