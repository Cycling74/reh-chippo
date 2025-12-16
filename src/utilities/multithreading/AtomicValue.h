/*
 ==============================================================================

    Atomic Value
    Author:  Dave Sanchez
    
 ==============================================================================
 */

#pragma once
#include "JuceHeader.h"

namespace nlt
{

using namespace juce;

template <typename Type>
struct AtomicValue
{
    AtomicValue() noexcept {}

    AtomicValue (Type t) noexcept
        : AtomicValue (t, t)
    {
    }

    AtomicValue (Type previous, Type current) noexcept
        : previousValue (previous)
        , currentValue (current)
        , atomicValue (current)
    {
    }

    // copy constructor for using this in an array
    AtomicValue (const AtomicValue& ap2) noexcept
        : AtomicValue (ap2.previousValue, ap2.currentValue)
    {
    }

    AtomicValue& operator= (const AtomicValue& toCopy) noexcept
    {
        currentValue  = toCopy.currentValue;
        previousValue = toCopy.previousValue;
        atomicValue.store (toCopy.atomicValue.load());
        return *this;
    }

    AtomicValue& operator= (const Type& newValue) noexcept
    {
        atomicValue.store (newValue);
        return *this;
    }

    Type exchange (const Type newValue) { return atomicValue.exchange (newValue); }

    Type updateCurrent() noexcept
    {
        currentValue = atomicValue.load();
        return currentValue;
    }

    Type updatePrevious() noexcept
    {
        auto prev     = previousValue;
        previousValue = currentValue;
        return prev;
    }

    Type load() const noexcept { return atomicValue.load(); }

    Type getPrevious() const noexcept { return previousValue; }

    Type getCurrent() const noexcept { return currentValue; }

    Type operator&() { return currentValue; }

    bool hasFreshValue (bool updatePrevious = true) noexcept
    {
        currentValue = atomicValue.load();
        bool isFresh = !juce::approximatelyEqual (currentValue, previousValue);
        if (updatePrevious)
            previousValue = currentValue;

        return isFresh;
    }

private:
    Type              previousValue { Type (0) };
    Type              currentValue { Type (0) };
    std::atomic<Type> atomicValue { Type (0) };

    JUCE_LEAK_DETECTOR (AtomicValue)
};

template <typename APType>
struct ScopedAtomicValue
{
    ScopedAtomicValue (AtomicValue<APType>& ap) noexcept
        : atomicValue (ap)
        , previous (ap.getPrevious())
        , current (ap.updateCurrent())
        , fresh (current != previous)
    {
    }

    ~ScopedAtomicValue() noexcept { atomicValue.updatePrevious(); }

    AtomicValue<APType>& atomicValue;
    APType               previous;
    APType               current;
    bool                 fresh;

    JUCE_LEAK_DETECTOR (ScopedAtomicValue)
};

template <typename T>
constexpr auto createScopedAtomicValue (AtomicValue<T>& object) noexcept
{
    return ScopedAtomicValue<T> (object);
}

// TODO: probably get rid of all this below

template <typename T, typename IntegerType>
constexpr auto createLinearSmoothedFromAtomicValue (ScopedAtomicValue<T>& scoped, IntegerType numSteps)
{
    juce::LinearSmoothedValue<T> smooth (scoped.previous);
    smooth.reset (static_cast<int> (numSteps));
    smooth.setTargetValue (scoped.current);
    return smooth;
}

template <typename T, typename IntegerType>
constexpr auto createLinearSmoothedFromAtomicValue (AtomicValue<T>& pair, IntegerType numSteps)
{
    auto scoped = createScopedAtomicValue (pair);
    return createLinearSmoothedFromAtomicValue (scoped, numSteps);
}

} // namespace nlt
