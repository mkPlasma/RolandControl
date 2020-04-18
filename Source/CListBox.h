#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"CListBoxListener.h"
#include"PluginProcessor.h"
#include"EffectList.h"
#include<string>
#include<vector>

using std::string;
using std::vector;

class RolandControlAudioProcessor;
class MidiOut;


class CListBox : public ListBox, public ListBoxModel{

	vector<string> contents_;
	CListBoxListener* listener_;
	bool forceRefresh_;

	RolandControlAudioProcessor& processor_;
	MidiOut& midiOut_;

	Effect effect_;

public:
	CListBox(RolandControlAudioProcessor& p, string name);
	CListBox(RolandControlAudioProcessor& p, string name, vector<string> cont);
	~CListBox();

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;

	void setContents(vector<string> cont);
	void loadSavedRow();

	void setListener(CListBoxListener* l);
	void selectedRowsChanged(int lastRowSelected) override;

	void setEffect(Effect effect);
};