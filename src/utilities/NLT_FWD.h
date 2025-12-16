/*
  ==============================================================================

    NLT_FWD.h

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
/**
 * Macro for easy std::forward use
 */
#define NLT_FWD(a) std::forward<decltype (a)> (a)
