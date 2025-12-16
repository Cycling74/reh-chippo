/*
 ==============================================================================

    Vector Tools

 ==============================================================================
 */
#pragma once
#include "JuceHeader.h"
#include "../NLT_FWD.h"

namespace nlt
{
namespace Vectors
{

    template <typename Type>
    static inline constexpr bool contains (std::vector<Type*>& vector, Type* ptr)
    {
        jassert (!vector.empty());
        if (vector.empty())
            return false;
        auto iterator   = std::find (vector.begin(), vector.end(), ptr);
        auto wasPresent = iterator != vector.end();
        return wasPresent;
    }

    template <typename Type, typename Fn>
    static inline constexpr bool contains (std::vector<Type>& vector, Fn&& compareFn)
    {
        static_assert (std::is_function_v<decltype (compareFn)>);

        auto iterator = std::find_if (vector.begin(), vector.end(), NLT_FWD (compareFn));
        auto present  = iterator != vector.end();
        return present;
    }

    template <typename Type>
    static inline constexpr bool contains (std::vector<Type>& vector, Type&& instanceToCompare)
    {
        static_assert (!std::is_function_v<decltype (instanceToCompare)>);
        auto iterator = std::find_if (vector.begin(),
                                      vector.end(),
                                      [comp = NLT_FWD (instanceToCompare)] (Type stored) { return stored == comp; });
        auto present  = iterator != vector.end();
        return present;
    }

    //    template <typename Type, typename Fn>
    //    static inline constexpr bool contains (std::vector<Type>& vector, Type&)
    //    {
    //        auto iterator = std::find_if (vector.begin(), vector.end(), NLT_FWD (compareFn));
    //        auto present  = iterator != vector.end();
    //        return present;
    //    }

    template <typename Type>
    static inline constexpr bool remove (std::vector<Type*>& vector, Type* ptr)
    {
        auto iterator   = std::find (vector.begin(), vector.end(), ptr);
        auto wasPresent = iterator != vector.end();
        if (wasPresent)
            vector.erase (iterator);

        return wasPresent;
    }

    template <typename Type>
    static inline constexpr bool addIfNotPresent (std::vector<Type*>& vector, Type* ptr)
    {
        auto iterator = std::find (vector.begin(), vector.end(), ptr);
        auto present  = iterator != vector.end();
        if (!present)
            vector.push_back (ptr);
        return !present;
    }

    template <typename Type, typename Fn>
    static inline constexpr bool addIfNotPresent (std::vector<Type*>& vector, Type ptr, Fn&& compareFn)
    {
        auto iterator = std::find_if (vector.begin(), vector.end(), NLT_FWD (compareFn));
        auto present  = iterator != vector.end();
        if (iterator == vector.end())
        {
            vector.push_back (ptr);
        }
        return !present;
    }

} // namespace Vectors
} // namespace nlt
