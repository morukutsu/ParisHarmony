#ifndef CHORDS_H_INCLUDED
#define CHORDS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <list>
#include <set>

#define PLUGIN_MAGIC "PAHR"

enum PluginFormatVersion
{
	V0 = 0,
};

struct NoteOff
{
	int cc, timestamp;
};

struct Note
{
	int cc, timestamp;
};

class Chords
{
public:
	Chords();

	void update(unsigned int numSamples, MidiBuffer& midiMessages);

	void holdNote(int cc);
	void dontHoldNote(int cc);

	int getMajorScale(int baseNote, int relative);

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
};



#endif  // CHORDS_H_INCLUDED
