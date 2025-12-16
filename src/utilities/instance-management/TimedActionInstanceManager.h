/*
 ==============================================================================

    GlobalTimedActionInstance
    Author:  Dave Sanchez

 ==============================================================================
 */

#pragma once
#include "InstanceManager.h"
#include "../../utilities/TimerAction.h"

namespace nlt
{
using namespace juce;

/**
 * Inherit from this to have a global manager call an action on all instances. Implement handleGlobalTimedAction()
 * and set the ActionRate in the template arg.
 *
 * @code
 * void handleGlobalTimedAction()
 * {
 *      // some action that will be called on all instances at UpdateRateHz rate
 * }
 * @tparam SubClass
 * @tparam ActionRateHz
 * @tparam ActionRateDenominator for rates slower than one Hz
 */
template <typename SubClass, int ActionRateHz, int ActionRateDenominator>
struct TimedActionInstance;

template <typename SubClass, int ActionRateHz, int ActionRateDenominator>
struct TimedActionInstanceManager : public InstanceManager<TimedActionInstance<SubClass, ActionRateHz, ActionRateDenominator>>
{
    TimedActionInstanceManager();
    ~TimedActionInstanceManager() override = default;

private:
    nlt::TimerAction timerAction;
    JUCE_LEAK_DETECTOR (TimedActionInstanceManager)
};

/**
 * Inherit from this to have all instances of the subclass have a method called by a singular timer.
 * Implement handleGlobalTimedAction() to use.
 * @tparam SubClass
 * @tparam ActionRateHz
 * @tparam ActionRateDenominator
 */
template <typename SubClass, int ActionRateHz, int ActionRateDenominator = 1>
struct TimedActionInstance : public ManagedInstance<TimedActionInstance<SubClass, ActionRateHz, ActionRateDenominator>,
                                                    TimedActionInstanceManager<SubClass, ActionRateHz, ActionRateDenominator>>
{
    TimedActionInstance()                   = default;
    virtual ~TimedActionInstance() override = default;

private:
    friend class TimedActionInstanceManager<SubClass, ActionRateHz, ActionRateDenominator>;
    void handleGlobalTimedActionInternal() { static_cast<SubClass*> (this)->handleGlobalTimedAction(); }
    JUCE_LEAK_DETECTOR (TimedActionInstance)
};

template <typename SubClass, int ActionRateHz, int ActionRateDenominator>
TimedActionInstanceManager<SubClass, ActionRateHz, ActionRateDenominator>::TimedActionInstanceManager()
{
    static_assert (ActionRateHz > 0 && ActionRateDenominator > 0, "should have a positive rate and denominator!");
    timerAction.setAction (
        [this]()
        {
            auto lock = this->getInstancesLock();
            ignoreUnused (lock);
            for (auto& instance: this->instances)
                instance->handleGlobalTimedActionInternal();
        },
        static_cast<float> (ActionRateHz) / static_cast<float> (ActionRateDenominator));
}

} // namespace nlt
