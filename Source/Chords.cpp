#include "Chords.h"

Chords::Chords()
{
	baseCC = 60;
	time = 0;
	scroll = 0;

	octaveMode = 0;
	chordsMode = 2;

	chordRecordId = -1;

	for (int i = 0; i < 128; i++)
		ccHeld[i] = false;

	for (int i = 0; i < CHORDS_COUNT; i++)
		for (int j = 0; j < NOTES_MAX; j++)
			chordsMem[i].notes[j] = -1;
}

void Chords::update(unsigned int numSamples, MidiBuffer& midiMessages)
{
	// Create a net of held notes using the buffer list
	std::set<int> notesHeld;
	for (int i = 0; i < 128; i++)
	{
		if (ccHeld[i])
		{
			notesHeld.insert(i);
		}
	}

	// Compare previous notes held and new ones to send notes on
	for (auto it = notesHeld.begin(); it != notesHeld.end(); it++)
	{
		auto foundNote = prevNotesHeld.find((*it));
		if (foundNote == prevNotesHeld.end())
		{
			// Detected a new note to send
			int note = *it;
			midiMessages.addEvent(MidiMessage::noteOn(1, note, (uint8)127), 0);
		}
	}

	// Compare current notes held and previous ones to send notes off
	for (auto it = prevNotesHeld.begin(); it != prevNotesHeld.end(); it++)
	{
		auto foundNote = notesHeld.find((*it));
		if (foundNote == notesHeld.end())
		{
			int note = *it;
			midiMessages.addEvent(MidiMessage::noteOff(1, note), 0);
		}
	}

	prevNotesHeld = notesHeld;
	time += numSamples;
}

void Chords::holdNote(int cc)
{
	ccHeld[cc] = true;
}

void Chords::dontHoldNote(int cc)
{
	ccHeld[cc] = false;
}

void Chords::holdRecordedChord(int chordIndex)
{
	for (int i = 0; i < NOTES_MAX; i++)
	{
		int note = chordsMem[chordIndex].notes[i];
		if (note != -1)
		{
			ccHeld[note] = true;
		}
	}
}

int Chords::getMajorScale(int baseNote, int relative)
{
	int scale[7] = { 0, 2, 4, 5, 7, 9, 11 };
	return baseNote + scale[relative];
}

void Chords::serialize(MemoryOutputStream& stream)
{
	// Write MAGIC and VERSION number
	for (int i = 0; i < 4; i++)
		stream.writeByte(PLUGIN_MAGIC[i]);
	stream.writeInt(static_cast<int>(PluginFormatVersion::V0));

	// Write content
	stream.writeInt(scroll);
	stream.writeInt(baseCC);
	stream.writeInt(octaveMode);
	stream.writeInt(chordsMode);
}

void Chords::unserialize(MemoryInputStream& stream)
{
	PluginFormatVersion version = detectFormatVersion(stream);
	scroll = stream.readInt();
	baseCC = stream.readInt();
	octaveMode = stream.readInt();
	chordsMode = stream.readInt();
}

PluginFormatVersion Chords::detectFormatVersion(MemoryInputStream& stream)
{
	stream.setPosition(0);

	// Read magic
	char magic[4];
	for (int i = 0; i < 4; i++)
		magic[i] = stream.readByte();

	// Read version from the stream
	PluginFormatVersion version = static_cast<PluginFormatVersion>(stream.readInt());
	return version;
}