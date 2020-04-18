#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"PluginProcessor.h"
#include"PluginEditor.h"
#include"CListBox.h"
#include"MidiOut.h"
#include"ToneList.h"
#include<string>
#include<vector>

using std::string;
using std::vector;

class RolandControlAudioProcessor;
class RolandControlAudioProcessorEditor;
class CListBox;
class MidiOut;
class ToneList;


class MToneList : public Component, public CListBoxListener{

	RolandControlAudioProcessor& processor_;
	RolandControlAudioProcessorEditor& editor_;
	MidiOut& midiOut_;
	ToneList& toneList_;

	int channel_;

	CListBox* lbCategories_;
	CListBox* lbTones_;

public:
	MToneList(RolandControlAudioProcessor&, RolandControlAudioProcessorEditor& e);
	~MToneList();

	void refresh();
	void setTones();

	void setChannel(int channel);

	void listBoxChanged(ListBox* lb, int row) override;
};