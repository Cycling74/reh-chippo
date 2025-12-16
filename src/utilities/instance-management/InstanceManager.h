/*
 ==============================================================================

    InstanceManager
    Author:  Dave Sanchez

 ==============================================================================
 */

#pragma once
#include "JuceHeader.h"
#include "../containers/VectorTools.h"

namespace nlt
{

/**
 * This will hold pointers to every instance of a specific class. Should be used as a base class for whatever it is that needs doing
 * @tparam InstanceClass
 */
template <typename ManagedInstance, typename LockType = juce::SpinLock>
struct InstanceManager
{
    InstanceManager() = default;

    virtual ~InstanceManager() = default;

    using ScopeLock = typename LockType::ScopedLockType;

    void addInstance (ManagedInstance* callback)
    {
        ScopeLock lock (instancesLock);
        auto      added = nlt::Vectors::addIfNotPresent (instances, callback);
        juce::ignoreUnused (added);
    }

    void removeInstance (ManagedInstance* callback)
    {
        ScopeLock lock (instancesLock);
        auto      removed = nlt::Vectors::remove (instances, callback);
        juce::ignoreUnused (removed);
    }

    long getNumInstances() const
    {
        ScopeLock lock (instancesLock);
        return static_cast<long> (instances.size());
    }

protected:
    LockType                      instancesLock;
    std::vector<ManagedInstance*> instances;

    std::unique_ptr<ScopeLock> getInstancesLock() { return std::make_unique<ScopeLock> (instancesLock); }

    JUCE_LEAK_DETECTOR (InstanceManager)
};

template <typename SubClass, typename InstanceManager, bool AddsSelf = true>
struct ManagedInstance
{
    ManagedInstance()
    {
        if constexpr (AddsSelf)
            instanceManager->addInstance (static_cast<SubClass*> (this));
    }
    virtual ~ManagedInstance()
    {
        if (removesSelf)
            removeFromInstanceManager();
    }

protected:
    bool isFirstManagedInstance() { return instanceManager->getNumInstances() == 1; }
    /**
     * Sets whether this instance should remove itself from the instance manager automatically at destruction.
     * If this is set to false, it's up to the subclass to remove itself at destruction if necessary
     *
     * @param shouldRemoveSelf  true if the instance should automatically remove itself at destruction
     */
    void setRemovesSelfFromInstanceManager (bool shouldRemoveSelf) { removesSelf = shouldRemoveSelf; }

    void removeFromInstanceManager() { instanceManager->removeInstance (static_cast<SubClass*> (this)); }

private:
    juce::SharedResourcePointer<InstanceManager> instanceManager;
    bool                                         removesSelf { true };

    JUCE_LEAK_DETECTOR (ManagedInstance)
};

} // namespace nlt
