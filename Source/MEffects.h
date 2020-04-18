#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"PluginProcessor.h"
#include"CSlider.h"
#include"CListBox.h"
#include<string>
#include<vector>

using std::string;
using std::vector;

class RolandControlAudioProcessor;
class CSlider;


class MEffects : public Component {

	RolandControlAudioProcessor& processor_;

	vector<CSlider*> sliders_;
	vector<AudioProcessorValueTreeState::SliderAttachment*> attachments_;
	vector<Label*> labels_;

	CListBox* lbReverb_;
	CListBox* lbChorus_;

	void addSlider(const string& name, string id, int x, int y);
	void addLabel(const string& text, int x, int y, bool large = false);

public:
	MEffects(RolandControlAudioProcessor&);
	~MEffects();
};