#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Gui.h"

//==============================================================================
ParisHarmonyAudioProcessorEditor::ParisHarmonyAudioProcessorEditor (ParisHarmonyAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	setSize(440, 300);
	startTimerHz(60);

	isMouseDown = isMouseClicked = isMouseDrag = false;
}

ParisHarmonyAudioProcessorEditor::~ParisHarmonyAudioProcessorEditor()
{
}

//==============================================================================
void ParisHarmonyAudioProcessorEditor::paint (Graphics& g)
{
	startGui();

	juce::Point<int> mousePos = getMouseXYRelative();
	g.fillAll(GREY_BG_COLOR);

	Font& f = g.getCurrentFont();
	f.setBold(true);
	g.setFont(f);
	
	bool isPlayedChord = false;

	for (int i = 0; i < 128; i++)
		notesHeldDuringFrame[i] = false;

	float notesAreaX = 64;
	float notesAreaY = 16;

	// Compute cursor pos
	const float noteW = 9;
	const float noteH = 32;

	float cursorW, cursorH;

	if (processor.mChords.octaveMode == 0)
	{
		cursorW = 64.0f;
	}
	else
	{
		cursorW = 128.0f;
	}

	if (processor.mChords.chordsMode == 0)
	{
		cursorH = 32.0f;
	}
	else if (processor.mChords.chordsMode == 1)
	{
		cursorH = 64.0f;
	}
	else if (processor.mChords.chordsMode == 2)
	{
		cursorH = 96.0f;
	}

	float inX = mousePos.x - notesAreaX;
	float inY = mousePos.y - notesAreaY;

	float cursorX = notesAreaX + ((int)inX / (int)noteW) * noteW;
	float cursorY = notesAreaY + ((int)inY / (int)noteH) * noteH;

	const float areaEndX = notesAreaX + noteW * (7 * 4);
	const float areaEndY = notesAreaY + noteH * (7);

	if (cursorX + cursorW > areaEndX)
	{
		cursorW -= (cursorX + cursorW) - areaEndX;

		if (cursorW < 0)
			cursorW = 0;
	}

	if (cursorY + cursorH > areaEndY)
	{
		cursorH -= (cursorY + cursorH) - areaEndY;

		if (cursorH < 0)
			cursorH = 0;
	}

	if (cursorX < notesAreaX)
	{
		cursorW = (cursorX + cursorW) - notesAreaX;
		cursorX = notesAreaX;
	}

	if (cursorY < notesAreaY)
	{
		cursorH = (cursorY + cursorH) - notesAreaY;
		cursorY = notesAreaY;
	}

	// Notes
	lowestNote = 128;
	lowestNoteRelative = -1;

	int relative = (processor.mChords.scroll * 5) % 7;
	for (int i = 0; i < 7; i++)
	{
		const float lineH = 32;
		paintLine(g, notesAreaX, notesAreaY + i * lineH, 6 - relative, cursorX, cursorY, cursorW, cursorH);

		relative += 2;
		relative = relative % 7;
	}

	// Notes text info
	relative = (processor.mChords.scroll * 5) % 7;
	for (int i = 0; i < 7; i++)
	{
		bool enabled = false;

		int note = processor.mChords.getScale(processor.mChords.baseCC, 6 - relative);

		char noteName[16];
		noteToStr(noteName, note);
		drawButton(&enabled, noteName, 16, i * noteH + notesAreaY + 8, 32, 16, g, mousePos.x, mousePos.y, isMouseDown, isMouseClicked);

		relative += 2;
		relative = relative % 7;
	}

	// Bottom controls

	// Offset
	drawKnobValue(&processor.mChords.scroll, 0, 127, 16, areaEndY + 10, 32, 16, NULL, g, mousePos.x, mousePos.y, isMouseDrag, mouseDragDistanceY);

	// Scale
	char* scaleNames[6] = { "MAJOR", "MINOR", "MAJOR H.", "MINOR H.", "MAJOR M.", "MINOR M." };
	drawKnobValue(&processor.mChords.currentScale, 0, 5, 16, areaEndY + 10 + 16 + 4, 80, 16, scaleNames[processor.mChords.currentScale],
		g, mousePos.x, mousePos.y, isMouseDrag, mouseDragDistanceY);

	// BaseCC
	char noteName[16];
	noteToStr(noteName, processor.mChords.baseCC);
	drawKnobValue(&processor.mChords.baseCC, 0, 127, 16 + 32 + 16, areaEndY + 10, 32, 16, noteName, g, mousePos.x, mousePos.y, isMouseDrag, mouseDragDistanceY);

	int x = 16 + 32 + 16 + 32 + 16;

	// 1 Note
	bool isChordsOne = processor.mChords.chordsMode == 0;
	bool isChordsTwo = processor.mChords.chordsMode == 1;
	bool isChordsThree = processor.mChords.chordsMode == 2;

	if (drawButton(&isChordsOne, "1- note", x, areaEndY + 10, 64, 16, g, mousePos.x, mousePos.y, isMouseDown, isMouseClicked))
		processor.mChords.chordsMode = 0;

	x += 64 + 16;

	if (drawButton(&isChordsTwo, "2- notes", x, areaEndY + 10, 64, 16, g, mousePos.x, mousePos.y, isMouseDown, isMouseClicked))
		processor.mChords.chordsMode = 1;

	x += 64 + 16;

	if (drawButton(&isChordsThree, "3- notes", x, areaEndY + 10, 64, 16, g, mousePos.x, mousePos.y, isMouseDown, isMouseClicked))
		processor.mChords.chordsMode = 2;

	x += 64 + 16;

	// Octave
	x = 16 + 32 + 16 + 32 + 16;

	bool isOctaveOne = processor.mChords.octaveMode == 0;
	bool isOctaveTwo = processor.mChords.octaveMode == 1;

	if (drawButton(&isOctaveOne, "1- oct", x, areaEndY + 10 + 16 + 4, 64, 16, g, mousePos.x, mousePos.y, isMouseDown, isMouseClicked))
		processor.mChords.octaveMode = 0;

	x += 64 + 16;

	if (drawButton(&isOctaveTwo, "2- oct", x, areaEndY + 10 + 16 + 4, 64, 16, g, mousePos.x, mousePos.y, isMouseDown, isMouseClicked))
		processor.mChords.octaveMode = 1;

	x += 64 + 16;
	
	// Chords memory
	for (int i = 0; i < CHORDS_COUNT; i++)
	{
		x = 330;
		int y = 28;
		bool enabled = processor.mChords.chordRecordId == i;
		if (drawClickableSquare(&enabled, x, y + i * 32, g, mousePos.x, mousePos.y, isMouseDown, isMouseClicked))
		{
			if (processor.mChords.chordRecordId == i)
				processor.mChords.chordRecordId = -1;
			else
				processor.mChords.chordRecordId = i;
		}

		// Mem button
		bool dummy = false;
		char buf[16];
		sprintf(buf, "MEM. %d", i + 1);
		if (drawButton(&dummy, buf, x + 28, y + i * 32, 64, 16, g, mousePos.x, mousePos.y, isMouseDown, isMouseClicked, true))
		{
			isPlayedChord = true;

			for (int j = 0; j < NOTES_MAX; j++)
			{
				int note = processor.mChords.chordsMem[i].notes[j];
				if (note != -1)
					notesHeldDuringFrame[note] = true;
			}
		}
	}

	// Chord info text
	if (lowestNoteRelative != -1 && lowestNoteRelative >= 0 && lowestNoteRelative < 7)
	{
		char* chordsMapping[] = { "I", "ii", "iii", "IV", "V", "vi", "vii°" };

		g.setColour(juce::Colour::fromRGBA(255, 255, 255, 255));
		g.drawText(chordsMapping[lowestNoteRelative], 400, 280, 40, 20, juce::Justification::centred);
	}


	// Cursor
	g.setColour(juce::Colour::fromRGBA(255, 255, 255, 96));
	g.fillRect(cursorX, cursorY, cursorW, cursorH);

	isMouseClicked = false;

	// Mouse pointer management
	if (getIsMouseOverKnob())
	{
		setMouseCursor(MouseCursor::UpDownResizeCursor);
	}
	else
	{
		setMouseCursor(MouseCursor::NormalCursor);
	}

	// Record notes to mem
	int inChordIndex = 0;
	bool clearMem = false;

	
	for (int i = 0; i < 128; i++)
	{
		if (!isPlayedChord)
		{
			if (notesHeldDuringFrame[i])
			{
				// Record chords
				if (processor.mChords.chordRecordId != -1)
				{
					if (!clearMem)
					{
						for (int j = 0; j < NOTES_MAX; j++)
							processor.mChords.chordsMem[processor.mChords.chordRecordId].notes[j] = -1;
						clearMem = true;
					}

					if (inChordIndex < NOTES_MAX)
					{
						processor.mChords.chordsMem[processor.mChords.chordRecordId].notes[inChordIndex] = i;
						inChordIndex++;
					}
				}
			}
		}

		processor.mChords.ccHeld[i] = notesHeldDuringFrame[i];
	}
}

bool rectCollision(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh)
{
	return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

int ParisHarmonyAudioProcessorEditor::paintNoteGroup(Graphics& g, int x, int y, int relative, float cursorX, float cursorY, float cursorW, float cursorH, int octave)
{
	int width = 0;

	for (int i = 0; i < 7; i++)
	{
		const float noteW = 8;
		const float noteH = 30;
		bool hover = false;

		float xPos = (float)x + i * noteW;
		float yPos = (float)y + 1;

		if (i == relative)
		{
			hover = rectCollision(xPos, yPos, noteW, noteH, cursorX, cursorY, cursorW, cursorH);

			int note = processor.mChords.getScale(processor.mChords.baseCC, relative);
			note += octave * 12;

			if (hover && !getInteractionsDisabled())
			{
				g.setColour(LOWLIGHT_COLOR);

				if (isMouseDown)
				{
					notesHeldDuringFrame[note] = true;
				}

				if (note < lowestNote)
				{
					lowestNote = note;
					lowestNoteRelative = relative;
				}
			}
			else
			{
				g.setColour(HIGHLIGHT_COLOR);
			}
		}
		else
			g.setColour(MEDIUM_GREY_COLOR);

		g.fillRect(xPos, yPos, noteW, noteH);

		x++;

		width += noteW + 1;
	}

	return width;
}

void ParisHarmonyAudioProcessorEditor::paintLine(Graphics& g, int x, int y, int relative, float cursorX, float cursorY, float cursorW, float cursorH)
{
	const float noteW  = 8 + 1;
	const float width  = noteW * (7 * 4) + 1;
	const float height = 32;

	g.setColour(LIGHT_GREY_COLOR);
	g.fillRect((float)x, (float)y, width, height);

	x++;
	x += paintNoteGroup(g, x, y, relative, cursorX, cursorY, cursorW, cursorH, 0);
	x += paintNoteGroup(g, x, y, relative, cursorX, cursorY, cursorW, cursorH, 1);
	x += paintNoteGroup(g, x, y, relative, cursorX, cursorY, cursorW, cursorH, 2);
	x += paintNoteGroup(g, x, y, relative, cursorX, cursorY, cursorW, cursorH, 3);
}

void ParisHarmonyAudioProcessorEditor::resized()
{
}

void ParisHarmonyAudioProcessorEditor::timerCallback()
{
	repaint();
}

void ParisHarmonyAudioProcessorEditor::mouseDown(const MouseEvent &event)
{
	isMouseDown = true;
}

void ParisHarmonyAudioProcessorEditor::mouseUp(const MouseEvent &event)
{
	if (isMouseDown)
	{
		isMouseClicked = true;
	}

	isMouseDown = false;
	isMouseDrag = false;
}

void ParisHarmonyAudioProcessorEditor::mouseDrag(const MouseEvent &event)
{
	isMouseDrag = true;
	mouseDragDistanceY = event.getDistanceFromDragStartY();
}

