#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class ParisHarmonyAudioProcessorEditor : public AudioProcessorEditor, public Timer
{
public:
    ParisHarmonyAudioProcessorEditor (ParisHarmonyAudioProcessor&);
    ~ParisHarmonyAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void timerCallback() override;

	void paintLine(Graphics& g, int x, int y, int relative, float cursorX, float cursorY, float cursorW, float cursorH);
	int paintNoteGroup(Graphics& g, int x, int y, int relative, float cursorX, float cursorY, float cursorW, float cursorH, int octave);

	void mouseDown(const MouseEvent &event) override;
	void mouseUp(const MouseEvent &event) override;
	void mouseDrag(const MouseEvent &event) override;

	bool isMouseDown, isMouseClicked, isMouseDrag;
	int mouseDragDistanceY;

	int lowestNote, lowestNoteRelative;

	bool notesHeldDuringFrame[128];

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ParisHarmonyAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParisHarmonyAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
