#pragma once
#include "RNBO.h"

#define NLT_IDT inline static const Identifier

namespace Sliders
{
using namespace juce;

NLT_IDT rootNote { "rootNote" };
NLT_IDT stepLength { "stepLength" };
NLT_IDT density { "density" };
NLT_IDT melodyLevel { "melodyLevel" };
NLT_IDT melodyOctave { "melodyOctave" };
NLT_IDT melodyWaveshape { "melodyWaveshape" };
NLT_IDT melodyAttack { "mainSynthVoice/melodyAttack" };
NLT_IDT melodyDecay { "mainSynthVoice/melodyDecay" };
NLT_IDT melodySustain { "mainSynthVoice/melodySustain" };
NLT_IDT melodyRelease { "mainSynthVoice/melodyRelease" };
NLT_IDT bassLevel { "bassLevel" };
NLT_IDT bassOctave { "bassOctave" };
NLT_IDT bassSlide { "BassLine/bassSlide" };
NLT_IDT bassAttack { "BassLine/bassAttack" };
NLT_IDT bassDecay { "BassLine/bassDecay" };
NLT_IDT bassSustain { "BassLine/bassSustain" };
NLT_IDT bassRelease { "BassLine/bassRelease" };
NLT_IDT kickLevel { "kickLevel" };
NLT_IDT snareLevel { "snareLevel" };
NLT_IDT hatLevel { "hatLevel" };
NLT_IDT reverbLevel { "reverbLevel" };

inline static const std::vector<Identifier> allIdts { melodyWaveshape, melodyOctave, bassOctave,   rootNote,    stepLength,
                                                      density,         melodyLevel,  melodyAttack, melodyDecay, melodySustain,
                                                      melodyRelease,   bassLevel,    bassSlide,    bassAttack,  bassDecay,
                                                      bassSustain,     bassRelease,  kickLevel,    snareLevel,  hatLevel,
                                                      reverbLevel };

inline static const std::vector<Identifier> trackGainIdts { melodyLevel, bassLevel, kickLevel, snareLevel, hatLevel };

inline static const std::vector<Identifier> octaveIdts { melodyOctave, bassOctave };

inline static const std::vector<Identifier> melodyIdts { melodyLevel, melodyOctave,  melodyWaveshape, melodyAttack,
                                                         melodyDecay, melodySustain, melodyRelease };

inline static const std::vector<Identifier> globalControlIdts { stepLength, rootNote, density, reverbLevel };

inline static const std::vector<Identifier> bassIdts { bassLevel, bassOctave,  bassSlide,  bassAttack,
                                                       bassDecay, bassSustain, bassRelease };

inline static const std::vector<Identifier> percIdts { kickLevel, snareLevel, hatLevel };

} // namespace Sliders

namespace Toggles
{
using namespace juce;

NLT_IDT run { "run" };
NLT_IDT generateMelodyAlways { "generateMelodyAlways" };

inline static const std::vector<Identifier> allIdts { run, generateMelodyAlways };

} // namespace Toggles


NLT_IDT sequencerVisIdt { "SequencerVisibility" };
namespace SeqButtons
{
using namespace juce;

NLT_IDT melody { "Melody" };
NLT_IDT bass { "Bass" };
NLT_IDT hat { "Hat" };
NLT_IDT kick { "Kick" };
NLT_IDT snare { "Snare" };
NLT_IDT clearMelody { "clearMelody" };
NLT_IDT clearBass { "clearBass" };
NLT_IDT clearKick { "clearKick" };
NLT_IDT clearSnare { "clearSnare" };
NLT_IDT clearHat { "clearHat" };

inline static const std::vector<Identifier> allIdts { melody,      bass,      kick,      snare,      hat,
                                                      clearMelody, clearBass, clearKick, clearSnare, clearHat };
inline static const std::vector<Identifier> genIdts { melody, bass, hat, snare, kick };
inline static const std::vector<Identifier> clearIdts { clearMelody, clearBass, clearKick, clearSnare, clearHat };

} // namespace SeqButtons
