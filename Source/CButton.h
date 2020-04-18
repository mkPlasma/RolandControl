#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"PluginProcessor.h"
#include"EffectList.h"
#include<string>

using std::string;

class RolandControlAudioProcessor;
class MidiOut;


class CButton : public ToggleButton, public ToggleButton::Listener {

	RolandControlAudioProcessor& processor_;
	MidiOut& midiOut_;

	Effect effect_;

public:
	CButton(RolandControlAudioProcessor& p, const string& id, int x, int y);

	void setEffect(Effect effect);
	void buttonClicked(Button* button) override;
};