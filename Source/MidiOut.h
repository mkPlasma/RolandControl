#pragma once

#include<windows.h>
#include<mmsystem.h>
#include<string>
#include"EffectList.h"

using std::string;

class RolandControlAudioProcessor;


class MidiOut{

	int midiDevNum_;
	bool open_;
	HMIDIOUT device_;

	RolandControlAudioProcessor& processor_;

	// Disabled due to crashing
	//void setStatusDW(DWORD& dw);
	//void setStatusBytes(char bytes[], int size);

public:
	MidiOut(RolandControlAudioProcessor& p);
	~MidiOut();

	void init();
	void reset();

	bool open();

	void shortMsg(DWORD data);

	void cc(int channel, int cc, int val);
	void setTone(int channel, int pc, int msb, int lsb);

	void sysexData(const string& address, char value);
	void sysexData(const string& address, const char* values, int size);

	void EFXPartSysex(int channel, Effect effect);
};