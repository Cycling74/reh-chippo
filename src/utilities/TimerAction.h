/*
 ==============================================================================

    Timer Action
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
 *  Executes a set function on a timed callback.
 *  Cleaner than inheriting from Timer
 */
struct TimerAction : private Timer
{
    /**
     * @tparam Fn           void() function
     * @param timerAction   action to execute
     * @param timerHz       interval to execute action at
     * @param start         start timer on instantiation
     */
    template <typename Fn>
    TimerAction (Fn&& timerAction, float timerHz, bool start = true)
        : timerActionFn (NLT_FWD (timerAction))
    {
        jassert (timerActionFn != nullptr); // need a function here
        jassert (timerHz > 0.0f);
        setTimerHz (timerHz);
        if (start)
            startTimer (timerMs);
    }

    TimerAction() = default;

    ~TimerAction() override = default;

    void timerCallback() override { timerActionFn(); }

    void setTimerHz (float timerHz, bool shouldStartTimer = true)
    {
        jassert (timerHz > 0.0f);
        setTimerMs (1000.0f / timerHz, shouldStartTimer);
    }

    void setTimerMs (int ms, bool shouldStartTimer = true)
    {
        timerMs = ms;
        if (shouldStartTimer)
            startTimer (timerMs);
    }

    void start (bool beginActionImmediately = false)
    {
        if (!isTimerRunning())
            startTimer (timerMs);
        if (beginActionImmediately)
            timerActionFn();
    }
    void stop() { stopTimer(); }

    template <typename Fn>
    void setAction (Fn&& fn, float timerHz = 0.0f, bool startAction = true)
    {
        timerActionFn = NLT_FWD (fn);
        jassert (timerActionFn != nullptr);
        if (timerHz > 0.0f && startAction)
            setTimerHz (timerHz);
    }

private:
    std::function<void()> timerActionFn { nullptr };
    int                   timerMs { 1000 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimerAction)
};

} // namespace nlt
