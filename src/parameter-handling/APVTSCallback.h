/*
  ==============================================================================

    ParameterAttachment.h
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include "instance-management/TimedActionInstanceManager.h"
#include "../../utilities/TimerAction.h"
#include "../../utilities/multithreading/AtomicValue.h"

namespace nlt
{
using namespace juce;

namespace NO_ACCESS
{
    struct APVTSCallbackBase : public AudioProcessorParameter::Listener
    {
        template <typename Callback>
        APVTSCallbackBase (RangedAudioParameter& _parameter, Callback&& _callback)
            : parameter (_parameter)
            , callback (NLT_FWD (_callback))
        {
        }

        ~APVTSCallbackBase() override { parameter.removeListener (this); }

        void parameterValueChanged (int parameterIndex, float newValue) override { ignoreUnused (parameterIndex, newValue); }

        void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
        {
            ignoreUnused (parameterIndex, gestureIsStarting);
        }

        virtual void setOfflineMode (bool isOffline) { ignoreUnused (isOffline); }

    protected:
        RangedAudioParameter&       parameter;
        std::function<void (float)> callback { nullptr };

        JUCE_LEAK_DETECTOR (APVTSCallbackBase)
    };

    struct APVTSCallbackInstance;

    struct APVTSCallbackInstanceManager : public InstanceManager<APVTSCallbackInstance>
    {
        APVTSCallbackInstanceManager()           = default;
        ~APVTSCallbackInstanceManager() override = default;

        static void setOfflineMode (bool isOffline)
        {
            SharedResourcePointer<APVTSCallbackInstanceManager> m;
            m->setOfflineModeInternal (isOffline);
        }

    private:
        void setOfflineModeInternal (bool isOffline);
        JUCE_LEAK_DETECTOR (APVTSCallbackInstanceManager)
    };

    struct APVTSCallbackInstance : public ManagedInstance<APVTSCallbackInstance, APVTSCallbackInstanceManager>,
                                   public APVTSCallbackBase
    {
        template <typename Callback>
        APVTSCallbackInstance (RangedAudioParameter& _parameter, Callback&& _callback)
            : APVTSCallbackBase (_parameter, NLT_FWD (_callback))
        {
            callback (parameter.getNormalisableRange().convertFrom0to1 (parameter.getValue()));
            parameter.addListener (this);
        }

        ~APVTSCallbackInstance() override = default;

        void setOfflineMode (bool isOffline) override { ignoreUnused (isOffline); }
    };

    struct APVTSCallbackSync : public APVTSCallbackInstance
    {
        template <typename Callback>
        APVTSCallbackSync (RangedAudioParameter& _parameter, Callback&& _callback)
            : APVTSCallbackInstance (_parameter, NLT_FWD (_callback))
        {
        }

        ~APVTSCallbackSync() override = default;

        void setOfflineMode (bool isOffline) override { ignoreUnused (isOffline); }

        void parameterValueChanged (int parameterIndex, float newValue) override
        {
            ignoreUnused (parameterIndex);
            callback (parameter.convertFrom0to1 (newValue));
        }

        void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
        {
            ignoreUnused (parameterIndex, gestureIsStarting);
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APVTSCallbackSync)
    };

    struct APVTSCallbackGUI : public APVTSCallbackInstance, public TimedActionInstance<APVTSCallbackGUI, 60, 1>
    {
        template <typename Callback>
        APVTSCallbackGUI (RangedAudioParameter& _parameter, Callback&& _callback)
            : APVTSCallbackInstance (_parameter, NLT_FWD (_callback))
        {
        }

        ~APVTSCallbackGUI() override = default;

        void handleGlobalTimedAction()
        {
            if (value.hasFreshValue())
                callback (value.updateCurrent());
        }

        void setOfflineMode (bool isOffline) override {}

        void parameterValueChanged (int parameterIndex, float newValue) override { value = parameter.convertFrom0to1 (newValue); }

        void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}

    private:
        AtomicValue<float> value { 0.0f };
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APVTSCallbackGUI)
    };

    struct APVTSCallbackAsync : public APVTSCallbackInstance, public TimedActionInstance<APVTSCallbackAsync, 60, 1>
    {
        template <typename Callback>
        APVTSCallbackAsync (RangedAudioParameter& _parameter, Callback&& _callback)
            : APVTSCallbackInstance (_parameter, NLT_FWD (_callback))
        {
        }

        ~APVTSCallbackAsync() override = default;

        void handleGlobalTimedAction()
        {
            if (value.hasFreshValue())
                callback (value.updateCurrent());
        }

        void setOfflineMode (bool isOffline) override
        {
            if (isOffline)
                parameter.removeListener (this);
            else
                parameter.addListener (this);
        }

        void parameterValueChanged (int parameterIndex, float newValue) override { value = parameter.convertFrom0to1 (newValue); }

        void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}

    private:
        AtomicValue<float> value { 0.0f };
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APVTSCallbackAsync)
    };

    struct APVTSCallbackAsyncOffline : public APVTSCallbackInstance
    {
        template <typename Callback>
        APVTSCallbackAsyncOffline (RangedAudioParameter& _parameter, Callback&& _callback)
            : APVTSCallbackInstance (_parameter, NLT_FWD (_callback))
        {
        }

        ~APVTSCallbackAsyncOffline() override = default;

        void setOfflineMode (bool isOffline) override
        {
            if (isOffline)
                parameter.addListener (this);
            else
                parameter.removeListener (this);
        }

        void parameterValueChanged (int parameterIndex, float newValue) override
        {
            callback (parameter.convertFrom0to1 (newValue));
        }

        void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APVTSCallbackAsyncOffline)
    };

    inline void APVTSCallbackInstanceManager::setOfflineModeInternal (bool isOffline)
    {
        auto lock = APVTSCallbackInstanceManager::getInstancesLock();
        for (auto& instance: APVTSCallbackInstanceManager::instances)
            static_cast<APVTSCallbackBase*> (instance)->setOfflineMode (isOffline);
    }
} // namespace NO_ACCESS

/**
 * Holds all your APVTS callbacks. MAKE SURE you use setAPVTS before adding any callbacks
 */
struct APVTSCallbacks
{
    APVTSCallbacks() = default;

    APVTSCallbacks (AudioProcessorValueTreeState& tree)
        : apvts (&tree)
    {
    }

    ~APVTSCallbacks() = default;

    void setOfflineMode (bool isOfflineMode) { NO_ACCESS::APVTSCallbackInstanceManager::setOfflineMode (isOfflineMode); }

    enum Type
    {
        /**
         * Use this for any callbacks that need to happen synchronously e.g. processing
         * parameter changes like gain values or w/e
         */
        sync = 0,
        /**
         * Use this for anything that needs to be triggered asynchronously on a timer.
         * When offline rendering is used, the same callback will happen synchronously
         */
        async,
        /**
         * Use this for any callbacks that solely operate on the GUI. These will not happen synchronously
         * in offline rendering mode. For that, use addAsynchronousCallback
         */
        gui
    };

    template <typename CallbackFn>
    void add (RangedAudioParameter& info, Type callbackType, CallbackFn&& callbackFn)
    {
        switch (callbackType)
        {
            case Type::sync:
                addSyncCallback (info, NLT_FWD (callbackFn));
                break;
            case Type::async:
                addAsyncCallback (info, NLT_FWD (callbackFn));
                break;
            case Type::gui:
                addGUICallback (info, NLT_FWD (callbackFn));
                break;
        }
    }

    //    template <typename CallbackFn>
    //    void add (const String& paramID, Type callbackType, CallbackFn&& callbackFn)
    //    {
    //        switch (callbackType)
    //        {
    //            case Type::sync:
    //                addSyncCallback (paramID, NLT_FWD (callbackFn));
    //                break;
    //            case Type::async:
    //                addAsyncCallback (paramID, NLT_FWD (callbackFn));
    //                break;
    //            case Type::gui:
    //                addGUICallback (paramID, NLT_FWD (callbackFn));
    //                break;
    //        }
    //    }

    void clear() { callbacks.clear(); }

private:
    AudioProcessorValueTreeState*            apvts { nullptr };
    OwnedArray<NO_ACCESS::APVTSCallbackBase> callbacks;

    /**
     * Use this for any callbacks that need to happen synchronously e.g. processing
     * parameter changes like gain values or w/e
     * @tparam Callback
     * @param info
     * @param callbackFn
     */
    template <typename Callback>
    void addSyncCallback (RangedAudioParameter& info, Callback&& callbackFn)
    {
        callbacks.add (new NO_ACCESS::APVTSCallbackSync (info, NLT_FWD (callbackFn)));
    }

    //    template <typename Callback>
    //    void addSyncCallback (const String& paramID, Callback&& callbackFn)
    //    {
    //        callbacks.add (
    //            new NO_ACCESS::APVTSCallbackSync (*apvts->getParameter (paramID), NLT_FWD (callbackFn)));
    //    }

    /**
     * Use this for anything that needs to be triggered asynchronously on a timer.
     * When offline rendering is used, the same callback will happen synchronously
     * @param info
     * @param callbackFn
     */
    void addAsyncCallback (RangedAudioParameter& info, std::function<void (float)> callbackFn)
    {
        callbacks.add (new NO_ACCESS::APVTSCallbackAsync (info, callbackFn));
        callbacks.add (new NO_ACCESS::APVTSCallbackAsyncOffline (info, callbackFn));
    }

    //    void addAsyncCallback (const String& paramID, std::function<void (float)> callbackFn)
    //    {
    //        callbacks.add (new NO_ACCESS::APVTSCallbackAsync (*apvts->getParameter (paramID), callbackFn));
    //        callbacks.add (new NO_ACCESS::APVTSCallbackAsyncOffline (*apvts->getParameter (paramID), callbackFn));
    //    }

    /**
     * Use this for any callbacks that solely operate on the GUI. These will not happen synchronously
     * in offline rendering mode. For that, use addAsynchronousCallback
     * @tparam Callback
     * @param info
     * @param callbackFn
     */
    template <typename Callback>
    void addGUICallback (RangedAudioParameter& info, Callback&& callbackFn)
    {
        callbacks.add (new NO_ACCESS::APVTSCallbackGUI (info, NLT_FWD (callbackFn)));
    }

    //    template <typename Callback>
    //    void addGUICallback (const String& paramID, Callback&& callbackFn)
    //    {
    //        callbacks.add (new NO_ACCESS::APVTSCallbackGUI (*apvts->getParameter (paramID), NLT_FWD (callbackFn)));
    //    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APVTSCallbacks)
};

} // namespace nlt
