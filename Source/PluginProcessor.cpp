#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ParisHarmonyAudioProcessor::ParisHarmonyAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

ParisHarmonyAudioProcessor::~ParisHarmonyAudioProcessor()
{
}

//==============================================================================
const String ParisHarmonyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ParisHarmonyAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ParisHarmonyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double ParisHarmonyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ParisHarmonyAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ParisHarmonyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ParisHarmonyAudioProcessor::setCurrentProgram (int index)
{
}

const String ParisHarmonyAudioProcessor::getProgramName (int index)
{
    return String();
}

void ParisHarmonyAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ParisHarmonyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ParisHarmonyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ParisHarmonyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ParisHarmonyAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	if (AudioPlayHead* ph = getPlayHead())
	{
		AudioPlayHead::CurrentPositionInfo newTime;

		if (ph->getCurrentPosition(newTime))
		{
			lastPosInfo = newTime;  // Successfully got the current time from the host..
		}
	}

	midiMessages.clear();

	mChords.update(buffer.getNumSamples(), midiMessages);
}

//==============================================================================
bool ParisHarmonyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ParisHarmonyAudioProcessor::createEditor()
{
    return new ParisHarmonyAudioProcessorEditor (*this);
}

//==============================================================================
void ParisHarmonyAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	MemoryOutputStream stream(destData, true);
	mChords.serialize(stream);
}

void ParisHarmonyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	MemoryInputStream stream(data, (size_t)sizeInBytes, false);
	mChords.unserialize(stream);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ParisHarmonyAudioProcessor();
}
