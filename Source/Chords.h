#ifndef CHORDS_H_INCLUDED
#define CHORDS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <list>
#include <set>

#define PLUGIN_MAGIC "PAHR"

enum PluginFormatVersion
{
	V0 = 0,
	V1 = 1,
	V2 = 2,
};

struct NoteOff
{
	int cc, timestamp;
};

struct Note
{
	int cc, timestamp;
};

const unsigned int CHORDS_COUNT = 7;
const unsigned int NOTES_MAX = 16;

struct ChordMemory
{
	int notes[NOTES_MAX]; // 16 notes max, -1 if disabled, otherwise midi note number
};

class Chords
{
public:
	Chords();

	void update(unsigned int numSamples, MidiBuffer& midiMessages);

	void holdNote(int cc);
	void dontHoldNote(int cc);
	void holdRecordedChord(int chordIndex);

	int getScale(int baseNote, int relative);

	void serialize(MemoryOutputStream& stream);
	void unserialize(MemoryInputStream& stream);
	PluginFormatVersion detectFormatVersion(MemoryInputStream& stream);

public:
	std::set<int> prevNotesHeld;
	std::list<Note> notesHeldBuffer;

	int baseCC;
	int time;

	int scroll;
	int octaveMode, chordsMode;

	bool ccHeld[128];

	ChordMemory chordsMem[CHORDS_COUNT];
	int chordRecordId;

	int currentScale;
};



#endif  // CHORDS_H_INCLUDED
