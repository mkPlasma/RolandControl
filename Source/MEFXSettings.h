#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"PluginProcessor.h"
#include"PluginEditor.h"
#include"CListBox.h"
#include"CSlider.h"
#include"MidiOut.h"
#include"EffectList.h"
#include<string>
#include<vector>

using std::string;
using std::vector;

class RolandControlAudioProcessor;
class RolandControlAudioProcessorEditor;
class CListBox;
class CSlider;
class MidiOut;
class EffectList;


class MEFXSettings : public Component, public CListBoxListener{

	RolandControlAudioProcessor& processor_;
	RolandControlAudioProcessorEditor& editor_;
	MidiOut& midiOut_;
	EffectList& effectList_;

	CListBox* lbCategories_;
	CListBox* lbEffects_;

	vector<CSlider*> sliders_;
	vector<AudioProcessorValueTreeState::SliderAttachment*> attachments_;
	vector<Label*> labels_;

	Effect effect_;
	Effect prevEffect_;

	void addSlider(const string& name, string id, int x, int y);
	void addLabel(const string& text, int x, int y, bool large = false);

public:
	MEFXSettings(RolandControlAudioProcessor&, RolandControlAudioProcessorEditor& e);
	~MEFXSettings();

	void refresh();

	void listBoxChanged(ListBox* lb, int row) override;
};