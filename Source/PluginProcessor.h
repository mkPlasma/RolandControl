#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"PluginEditor.h"
#include"MidiOut.h"
#include"ToneList.h"
#include"EffectList.h"
#include<string>

#define INIT_WAIT 500

using std::string;

class MidiOut;
class ToneList;
class EffectList;


//==============================================================================
/**
*/
class RolandControlAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    RolandControlAudioProcessor();
    ~RolandControlAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


	MidiOut& getMidiOut();
	ToneList& getToneList();
	EffectList& getEffectList();

	AudioProcessorValueTreeState& getParameters();

private:

	void setStatus(const string& status, const Colour& colour = Colours::white);
    void initMIDI();

	MidiOut midiOut_;

    bool initialized_;

	ToneList toneList_;
	EffectList effectList_;

	AudioProcessorValueTreeState parameters_;

    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RolandControlAudioProcessor)
};
