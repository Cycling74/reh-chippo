/*
  ==============================================================================

    ValeTreeCallback.h
    Author:  Dave Sanchez

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>

namespace DeduceTypeFromLambdaFunctionArg
{
template <typename T>
struct functionType
{
    using type = void;
};

template <typename Ret, typename Class, typename... Args>
struct functionType<Ret (Class::*) (Args...) const>
{
    using type = std::function<Ret (Args...)>;
};

template <typename F>
typename functionType<decltype (&F::operator())>::type functionFromLambda (F const& func)
{ // Function from lambda !
    return func;
}
} // namespace DeduceTypeFromLambdaFunctionArg

namespace NO_ACCESS
{
/**
     * Value Tree Callback base type for holding all template types
     * in an owned array. Don't use!
     */
struct ValueTreeCallbackBase
{
    ValueTreeCallbackBase()          = default;
    virtual ~ValueTreeCallbackBase() = default;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeCallbackBase)
};

} // namespace NO_ACCESS

/**
 * Don't use this directly. Use ValueTreeCallbacks
 * @tparam Type
 */
template <typename Type>
class ValueTreeCallback : public NO_ACCESS::ValueTreeCallbackBase, public ValueTree::Listener
{
public:
    ~ValueTreeCallback() override = default;

    void valueTreePropertyChanged (ValueTree& tree, const Identifier& changedProperty) override
    {
        if (tree.getType() == valueTree.getType() && changedProperty == property)
            callback (static_cast<Type> (tree.getProperty (property)));
    }

private:
    ValueTree                  valueTree;
    const Identifier           property;
    std::function<void (Type)> callback;

    template <typename Fn>
    ValueTreeCallback (ValueTree tree, const juce::Identifier& _property, Fn&& callbackToUse, bool init = true)
        : valueTree (tree)
        , property (_property)
        , callback (NLT_FWD (callbackToUse))
    {
        valueTree.addListener (this);
        jassert (callback != nullptr);
        if (init)
            callback (static_cast<Type> (tree.getProperty (property)));
    }

    friend class ValueTreeCallbacks;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeCallback)
};

/**
 * ValueTreeCallbacks
 * Holds all your callbacks in one neat container.
 * Use the add function to add any callbacks you might need.
 */
class ValueTreeCallbacks
{
public:
    ValueTreeCallbacks()  = default;
    ~ValueTreeCallbacks() = default;

    template <typename Fn>
    void add (ValueTree tree, const juce::Identifier& property, Fn&& callbackFn, bool init = true)
    {
        callbacks.add (createCallback (tree,
                                       property,
                                       std::move (DeduceTypeFromLambdaFunctionArg::functionFromLambda (NLT_FWD (callbackFn))),
                                       init));
    }

    void clear() { callbacks.clear(); }

private:
    OwnedArray<NO_ACCESS::ValueTreeCallbackBase> callbacks;

    template <typename Type>
    static auto
        createCallback (ValueTree tree, const juce::Identifier& property, std::function<void (Type)>&& callback, bool init)
    {
        return new ValueTreeCallback<Type> (tree, property, NLT_FWD (callback), init);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeCallbacks)
};
