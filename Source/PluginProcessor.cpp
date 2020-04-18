#include"PluginProcessor.h"
#include"HexFunc.h"

using std::make_unique;
using std::unique_ptr;



//==============================================================================
RolandControlAudioProcessor::RolandControlAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
     AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
	midiOut_(*this), toneList_(), effectList_(), initialized_(false),
	
	parameters_(*this, nullptr, Identifier("RolandControlParameters"),
		{
			make_unique<AudioParameterInt>("channel",	"Channel",	0,	16,	0),
			//make_unique<AudioParameterInt>("tab",		"Tab",	0,	16,	0),

			make_unique<AudioParameterInt>("cc73ch0",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch1",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch2",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch3",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch4",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch5",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch6",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch7",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch8",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch9",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch10",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch11",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch12",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch13",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch14",	"Env Attack",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc73ch15",	"Env Attack",	-64,	63,	0),

			make_unique<AudioParameterInt>("cc75ch0",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch1",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch2",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch3",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch4",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch5",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch6",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch7",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch8",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch9",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch10",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch11",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch12",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch13",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch14",	"Env Decay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc75ch15",	"Env Decay",	-64,	63,	0),

			make_unique<AudioParameterInt>("cc72ch0",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch1",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch2",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch3",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch4",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch5",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch6",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch7",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch8",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch9",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch10",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch11",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch12",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch13",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch14",	"Env Release",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc72ch15",	"Env Release",	-64,	63,	0),


			make_unique<AudioParameterInt>("cc77ch0",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch1",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch2",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch3",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch4",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch5",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch6",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch7",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch8",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch9",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch10",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch11",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch12",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch13",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch14",	"Vib Depth",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc77ch15",	"Vib Depth",	-64,	63,	0),

			make_unique<AudioParameterInt>("cc76ch0",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch1",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch2",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch3",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch4",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch5",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch6",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch7",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch8",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch9",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch10",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch11",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch12",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch13",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch14",	"Vib Rate",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc76ch15",	"Vib Rate",	-64,	63,	0),

			make_unique<AudioParameterInt>("cc78ch0",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch1",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch2",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch3",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch4",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch5",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch6",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch7",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch8",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch9",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch10",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch11",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch12",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch13",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch14",	"Vib Delay",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc78ch15",	"Vib Delay",	-64,	63,	0),
			

			make_unique<AudioParameterInt>("cc74ch0",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch1",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch2",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch3",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch4",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch5",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch6",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch7",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch8",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch9",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch10",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch11",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch12",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch13",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch14",	"Filter Cutoff",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc74ch15",	"Filter Cutoff",	-64,	63,	0),

			make_unique<AudioParameterInt>("cc71ch0",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch1",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch2",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch3",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch4",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch5",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch6",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch7",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch8",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch9",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch10",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch11",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch12",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch13",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch14",	"Filter Resonance",	-64,	63,	0),
			make_unique<AudioParameterInt>("cc71ch15",	"Filter Resonance",	-64,	63,	0),
				
			make_unique<AudioParameterBool>("cc65ch0",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch1",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch2",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch3",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch4",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch5",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch6",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch7",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch8",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch9",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch10",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch11",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch12",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch13",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch14",	"Porta OnOff",	false),
			make_unique<AudioParameterBool>("cc65ch15",	"Porta OnOff",	false),

			make_unique<AudioParameterInt>("cc5ch0",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch1",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch2",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch3",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch4",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch5",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch6",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch7",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch8",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch9",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch10",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch11",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch12",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch13",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch14",	"Porta Time",	0,	127,	0),
			make_unique<AudioParameterInt>("cc5ch15",	"Porta Time",	0,	127,	0),


			make_unique<AudioParameterInt>("cc1ch0",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch1",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch2",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch3",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch4",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch5",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch6",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch7",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch8",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch9",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch10",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch11",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch12",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch13",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch14",	"Mod Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc1ch15",	"Mod Amt",	0,	127,	0),


			make_unique<AudioParameterInt>("cc91ch0",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch1",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch2",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch3",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch4",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch5",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch6",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch7",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch8",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch9",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch10",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch11",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch12",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch13",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch14",	"Reverb Amt",	0,	127,	40),
			make_unique<AudioParameterInt>("cc91ch15",	"Reverb Amt",	0,	127,	40),

			make_unique<AudioParameterInt>("cc93ch0",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch1",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch2",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch3",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch4",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch5",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch6",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch7",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch8",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch9",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch10",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch11",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch12",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch13",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch14",	"Chorus Amt",	0,	127,	0),
			make_unique<AudioParameterInt>("cc93ch15",	"Chorus Amt",	0,	127,	0),
			/*
			make_unique<AudioParameterInt>("catSel0",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel1",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel2",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel3",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel4",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel5",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel6",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel7",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel8",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel9",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel10",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel11",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel12",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel13",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel14",	"Tone Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("catSel15",	"Tone Category Selected",	0,	127,	0),
			*/
			make_unique<AudioParameterInt>("cat0",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat1",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat2",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat3",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat4",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat5",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat6",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat7",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat8",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat9",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat10",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat11",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat12",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat13",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat14",	"Tone Category",	0,	127,	0),
			make_unique<AudioParameterInt>("cat15",	"Tone Category",	0,	127,	0),

			make_unique<AudioParameterInt>("tone0",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone1",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone2",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone3",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone4",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone5",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone6",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone7",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone8",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone9",		"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone10",	"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone11",	"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone12",	"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone13",	"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone14",	"Tone",	0,	127,	0),
			make_unique<AudioParameterInt>("tone15",	"Tone",	0,	127,	0),
			
			make_unique<AudioParameterBool>("toneSet0",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet1",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet2",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet3",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet4",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet5",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet6",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet7",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet8",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet9",		"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet10",	"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet11",	"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet12",	"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet13",	"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet14",	"Tone set",	false),
			make_unique<AudioParameterBool>("toneSet15",	"Tone set",	false),
			
			make_unique<AudioParameterInt>("efxDepthch0",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch1",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch2",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch3",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch4",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch5",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch6",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch7",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch8",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch9",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch10",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch11",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch12",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch13",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch14",		"Part EFX Depth",	0,	127,	0),
			make_unique<AudioParameterInt>("efxDepthch15",		"Part EFX Depth",	0,	127,	0),

			make_unique<AudioParameterInt>("efxParam1ch0",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch1",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch2",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch3",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch4",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch5",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch6",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch7",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch8",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch9",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch10",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch11",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch12",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch13",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch14",		"Part EFX Parameter 1",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam1ch15",		"Part EFX Parameter 1",		-1,	127,	-1),
			
			make_unique<AudioParameterInt>("efxParam2ch0",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch1",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch2",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch3",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch4",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch5",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch6",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch7",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch8",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch9",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch10",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch11",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch12",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch13",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch14",		"Part EFX Parameter 2",		-1,	127,	-1),
			make_unique<AudioParameterInt>("efxParam2ch15",		"Part EFX Parameter 2",		-1,	127,	-1),
			
			make_unique<AudioParameterInt>("sysex 40 01 30",	"Reverb Macro",				0,	127,	4),
			make_unique<AudioParameterInt>("sysex 40 01 32",	"Reverb Pre-LPF",			0,	7,		0),
			make_unique<AudioParameterInt>("sysex 40 01 33",	"Reverb Level",				0,	127,	64),
			make_unique<AudioParameterInt>("sysex 40 01 34",	"Reverb Time",				0,	127,	64),
			make_unique<AudioParameterInt>("sysex 40 01 35",	"Reverb Feedback",			0,	127,	0),
			
			make_unique<AudioParameterInt>("sysex 40 01 38",	"Chorus Macro",				0,	127,	2),
			make_unique<AudioParameterInt>("sysex 40 01 39",	"Chorus Pre-LPF",			0,	7,		0),
			make_unique<AudioParameterInt>("sysex 40 01 3a",	"Chorus Level",				0,	127,	64),
			make_unique<AudioParameterInt>("sysex 40 01 3b",	"Chorus Feedback",			0,	127,	8),
			make_unique<AudioParameterInt>("sysex 40 01 3c",	"Chorus Delay",				0,	127,	80),
			make_unique<AudioParameterInt>("sysex 40 01 3d",	"Chorus Rate",				0,	127,	3),
			make_unique<AudioParameterInt>("sysex 40 01 3e",	"Chorus Depth",				0,	127,	19),
			make_unique<AudioParameterInt>("sysex 40 01 3f",	"Chorus Send",				0,	127,	0),
			
			make_unique<AudioParameterInt>("efxCat",			"EFX Category",				0,	127,	0),
			//make_unique<AudioParameterInt>("efxCatSel",			"EFX Category Selected",	0,	127,	0),
			make_unique<AudioParameterInt>("efx",				"EFX Type",					0,	127,	0),
			make_unique<AudioParameterBool>("efxSet",			"EFX Set",					false),
			make_unique<AudioParameterInt>("sysex 40 03 1a",	"EFX Depth",				0,	127,	127),
			make_unique<AudioParameterInt>("sysex 40 03 17",	"EFX Reverb Send",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 18",	"EFX Chorus Send",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 03",	"EFX Parameter 1",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 04",	"EFX Parameter 2",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 05",	"EFX Parameter 3",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 06",	"EFX Parameter 4",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 07",	"EFX Parameter 5",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 08",	"EFX Parameter 6",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 09",	"EFX Parameter 7",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 0a",	"EFX Parameter 8",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 0b",	"EFX Parameter 9",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 0c",	"EFX Parameter 10",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 0d",	"EFX Parameter 11",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 0e",	"EFX Parameter 12",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 0f",	"EFX Parameter 13",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 10",	"EFX Parameter 14",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 11",	"EFX Parameter 15",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 12",	"EFX Parameter 16",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 13",	"EFX Parameter 17",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 14",	"EFX Parameter 18",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 15",	"EFX Parameter 19",			0,	127,	0),
			make_unique<AudioParameterInt>("sysex 40 03 16",	"EFX Parameter 20",			0,	127,	0),
		}
	)
{
}

RolandControlAudioProcessor::~RolandControlAudioProcessor(){
	
}

//==============================================================================
const String RolandControlAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RolandControlAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RolandControlAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RolandControlAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RolandControlAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RolandControlAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RolandControlAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RolandControlAudioProcessor::setCurrentProgram (int index)
{
}

const String RolandControlAudioProcessor::getProgramName (int index)
{
    return {};
}

void RolandControlAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void RolandControlAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void RolandControlAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RolandControlAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void RolandControlAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages){
	/*
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
	*/

	buffer.clear();

	if(!midiOut_.open())
		return;

	// Wait a few cycles before initializing to prevent crashing
	if(!initialized_){
		initMIDI();
		initialized_ = true;
	}

	MidiBuffer processedMidi;
	int time;
	MidiMessage m;

	for(MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);){

		// Short message
		if(m.getRawDataSize() == 3){
			union{ DWORD dwData; BYTE bData[4]; } msg;

			for(int i = 0; i < 3; i++)
				msg.bData[i] = m.getRawData()[i];
			msg.bData[3] = 0;

			midiOut_.shortMsg(msg.dwData);

			if(getActiveEditor() != nullptr){

				if(m.isNoteOn())
					((RolandControlAudioProcessorEditor*)getActiveEditor())->noteOn(m.getChannel() - 1, m.getNoteNumber());
				else if(m.isNoteOff())
					((RolandControlAudioProcessorEditor*)getActiveEditor())->noteOff(m.getChannel() - 1, m.getNoteNumber());
			}
		}

		//processedMidi.addEvent(m, time);
	}

	//midiMessages.swapWith(processedMidi);
}

void RolandControlAudioProcessor::initMIDI(){

	// Send MIDI parameters

	// CC
	int cc[] = {1, 5, 65, 71, 72, 73, 74, 75, 76, 77, 78, 91, 93};

	for(int i : cc){
		for(int j = 0; j < 16; j++){
			string id = "cc" + std::to_string(i) + "ch" + std::to_string(j);
			int val = (int)*parameters_.getRawParameterValue(id);

			// Adjust value for parameters that can be negative
			val += parameters_.getParameter(id)->getNormalisableRange().getRange().getStart() < 0 ? 64 : 0;

			midiOut_.cc(j, i, val);
		}
	}


	// Tones
	for(int i = 0; i < 16; i++){

		// If tone is set, send MIDI
		if((bool)*parameters_.getRawParameterValue("toneSet" + std::to_string(i))){
			Tone tone = toneList_.getTones()[
				(int)*parameters_.getRawParameterValue("cat" + std::to_string(i))
			].tones[(int)*parameters_.getRawParameterValue("tone" + std::to_string(i))];

				midiOut_.setTone(i, tone.pc, tone.msb, tone.lsb);
		}
	}


	// Effects
	midiOut_.sysexData("40 01 30", (int)*parameters_.getRawParameterValue("sysex 40 01 30"));
	midiOut_.sysexData("40 01 32", (int)*parameters_.getRawParameterValue("sysex 40 01 32"));
	midiOut_.sysexData("40 01 33", (int)*parameters_.getRawParameterValue("sysex 40 01 33"));
	midiOut_.sysexData("40 01 34", (int)*parameters_.getRawParameterValue("sysex 40 01 34"));
	midiOut_.sysexData("40 01 35", (int)*parameters_.getRawParameterValue("sysex 40 01 35"));

	midiOut_.sysexData("40 01 38", (int)*parameters_.getRawParameterValue("sysex 40 01 38"));
	midiOut_.sysexData("40 01 39", (int)*parameters_.getRawParameterValue("sysex 40 01 39"));
	midiOut_.sysexData("40 01 3a", (int)*parameters_.getRawParameterValue("sysex 40 01 3a"));
	midiOut_.sysexData("40 01 3b", (int)*parameters_.getRawParameterValue("sysex 40 01 3b"));
	midiOut_.sysexData("40 01 3c", (int)*parameters_.getRawParameterValue("sysex 40 01 3c"));
	midiOut_.sysexData("40 01 3d", (int)*parameters_.getRawParameterValue("sysex 40 01 3d"));
	midiOut_.sysexData("40 01 3e", (int)*parameters_.getRawParameterValue("sysex 40 01 3e"));
	midiOut_.sysexData("40 01 3f", (int)*parameters_.getRawParameterValue("sysex 40 01 3f"));


	// EFX
	if((bool)*parameters_.getRawParameterValue("efxSet")){
		int cat = (int)*parameters_.getRawParameterValue("efxCat");
		int efx = (int)*parameters_.getRawParameterValue("efx");
		Effect effect;

		if(cat >= effectList_.getEffects().size() || efx >= effectList_.getEffects()[cat].effects.size())
			effect = effectList_.getEffects()[0].effects[0];
		else
			effect = effectList_.getEffects()[cat].effects[efx];

		char data[] = {effect.msb, effect.lsb};
		midiOut_.sysexData("40 03 00", data, sizeof(data));

		// Set EFX depth and send levels
		midiOut_.sysexData("40 03 1a", (int)*parameters_.getRawParameterValue("sysex 40 03 1a"));
		midiOut_.sysexData("40 03 17", (int)*parameters_.getRawParameterValue("sysex 40 03 17"));
		midiOut_.sysexData("40 03 18", (int)*parameters_.getRawParameterValue("sysex 40 03 18"));

		// Part parameters
		for(int i = 0; i < 16; i++)
			midiOut_.EFXPartSysex(i, effect);

		// Parameters
		for(int i = 0; i < 20; i++){
			char address[32];
			unsigned char c = i + 3;
			sprintf(address, "40 03 %s", byteToHex(c).c_str());

			midiOut_.sysexData(address, (int)*parameters_.getRawParameterValue("sysex " + (string)address));
		}
	}
}

//==============================================================================
bool RolandControlAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* RolandControlAudioProcessor::createEditor()
{
    return new RolandControlAudioProcessorEditor(*this);
}

//==============================================================================
void RolandControlAudioProcessor::getStateInformation (MemoryBlock& destData){
	auto state = parameters_.copyState();
	unique_ptr<XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void RolandControlAudioProcessor::setStateInformation (const void* data, int sizeInBytes){
	unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if(xmlState.get() != nullptr)
		if(xmlState->hasTagName(parameters_.state.getType()))
			parameters_.replaceState(ValueTree::fromXml(*xmlState));
}

//==============================================================================
void RolandControlAudioProcessor::setStatus(const string& status, const Colour& colour){
	//if(getActiveEditor() != nullptr)
	//		((RolandControlAudioProcessorEditor*)getActiveEditor())->setStatus(status, colour);
}

MidiOut& RolandControlAudioProcessor::getMidiOut(){
	return midiOut_;
}

ToneList& RolandControlAudioProcessor::getToneList(){
	return toneList_;
}

EffectList& RolandControlAudioProcessor::getEffectList(){
	return effectList_;
}

AudioProcessorValueTreeState& RolandControlAudioProcessor::getParameters(){
	return parameters_;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RolandControlAudioProcessor();
}