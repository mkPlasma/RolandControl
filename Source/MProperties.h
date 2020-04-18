#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"PluginProcessor.h"
#include"CSlider.h"
#include"CButton.h"
#include"MidiOut.h"
#include"EffectList.h"
#include<string>
#include<vector>

using std::string;
using std::vector;

class RolandControlAudioProcessor;
class MidiOut;
class CSlider;
class CButton;


class MProperties : public Component, public Slider::Listener{

	RolandControlAudioProcessor& processor_;
	MidiOut& midiOut_;

	Effect effect_;
	
	vector<CSlider*> sliders_;
	vector<CButton*> buttons_;
	vector<AudioProcessorValueTreeState::SliderAttachment*> attachments_;
	vector<AudioProcessorValueTreeState::ButtonAttachment*> bAttachments_;
	vector<Label*> labels_;

	Label* efxLabel_;
	Label* efxParam1Label_;
	Label* efxParam2Label_;

	Label* addSlider(const string& name, string id, int x, int y, bool efx = false, bool toggle = false);
	void addButton(const string& name, const string& id, int x, int y);
	Label* addLabel(const string& text, int x, int y, bool large = false);

public:
	MProperties(RolandControlAudioProcessor&);
	~MProperties();

	void setChannel(int channel);
	void setEffect(Effect effect, bool useDefaultValues);
	void sliderValueChanged(Slider* slider) override;
};