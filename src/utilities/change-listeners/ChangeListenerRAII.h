/*
  ==============================================================================

    ChangeListenerRAII.h
    Created: 15 Dec 2022 5:01:22pm
    Author:  David Sanchez

  ==============================================================================
*/
#pragma once
#include "JuceHeader.h"
#include "../containers/VectorTools.h"

namespace nlt
{

struct ChangeListenerRAII : public ChangeListener
{
    ChangeListenerRAII() = default;
    virtual ~ChangeListenerRAII() override { removeFromAllBroadcasters(); }

    void addAsChangeListenerTo (juce::ChangeBroadcaster& broadcaster) &
    {
        broadcaster.addChangeListener (this);
        broadcasters.push_back (&broadcaster);
    }

    bool removeAsChangeListenerFrom (juce::ChangeBroadcaster& broadcaster) &
    {
        broadcaster.removeChangeListener (this);
        return Vectors::remove (broadcasters, &broadcaster);
    }

    // ============== copy and move assignments/constructors
    ChangeListenerRAII& operator= (const ChangeListenerRAII& other) noexcept
    {
        removeFromAllBroadcasters();
        broadcasters = other.broadcasters;
        addToAllBroadcasters();
        return *this;
    }
    ChangeListenerRAII (const ChangeListenerRAII& other) noexcept { *this = other; }

    ChangeListenerRAII& operator= (ChangeListenerRAII&& other) noexcept
    {
        removeFromAllBroadcasters();
        other.removeFromAllBroadcasters();
        broadcasters = std::move (other.broadcasters);
        addToAllBroadcasters();
        return *this;
    }
    ChangeListenerRAII (ChangeListenerRAII&& other) noexcept { *this = std::move (other); }

protected:
    std::vector<juce::ChangeBroadcaster*> broadcasters;

    void removeFromAllBroadcasters()
    {
        for (auto& b: broadcasters)
            b->removeChangeListener (this);
    }

    void addToAllBroadcasters()
    {
        for (auto& b: broadcasters)
            b->addChangeListener (this);
    }

    JUCE_LEAK_DETECTOR (ChangeListenerRAII)
};

} // namespace nlt