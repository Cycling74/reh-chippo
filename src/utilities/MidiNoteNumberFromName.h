/*
  ==============================================================================

    MidiNoteNumberFromName.h
    Author:  Dave Sanchez

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace nlt
{
using namespace juce;

inline static int midiNoteNumberFromName (String name, int midiMiddleCOctave = 4)
{
    if (name.trim().isEmpty())
        return -1;

    name = name.trim();
    name = name.replace (" ", "");
    name = name.replaceCharacter (juce::CharPointer_UTF8 ("\u266F")[0], '#');
    name = name.replaceCharacter (juce::CharPointer_UTF8 ("\u266D")[0], 'b');
    name = name.toUpperCase();

    if (name.isEmpty())
        return -1;

    juce_wchar first = name[0];
    if (first < 'A' || first > 'G')
        return -1;

    int semitone = 0;
    switch (first)
    {
        case 'C':
            semitone = 0;
            break;
        case 'D':
            semitone = 2;
            break;
        case 'E':
            semitone = 4;
            break;
        case 'F':
            semitone = 5;
            break;
        case 'G':
            semitone = 7;
            break;
        case 'A':
            semitone = 9;
            break;
        case 'B':
            semitone = 11;
            break;
        default:
            return -1;
    }

    int pos = 1;
    while (pos < name.length())
    {
        juce_wchar c = name[pos];
        if (c == '#' || c == 'X')
        {
            semitone += 1;
            ++pos;
        }
        else if (c == 'B')
        {
            semitone -= 1;
            ++pos;
        }
        else
            break;
    }

    String octStr = name.substring (pos);
    if (octStr.isEmpty())
        return -1;

    bool validOct = true;
    for (int i = 0; i < octStr.length(); ++i)
    {
        juce_wchar ch = octStr[i];
        if (i == 0 && (ch == '+' || ch == '-'))
            continue;
        if (ch < '0' || ch > '9')
        {
            validOct = false;
            break;
        }
    }
    if (!validOct)
        return -1;

    int octave = octStr.getIntValue();

    while (semitone < 0)
    {
        semitone += 12;
        --octave;
    }
    while (semitone >= 12)
    {
        semitone -= 12;
        ++octave;
    }

    int midi = (octave + 1) * 12 + semitone;

    if (midi < 0 || midi > 127)
        return -1;

    return midi;
}

} // namespace nlt
