#include "MEffects.h"

#include<regex>

using std::regex;
using std::smatch;
using std::regex_search;


MEffects::MEffects(RolandControlAudioProcessor& p) : processor_(p) {

	addLabel("Reverb", 10, 10, true);

	vector<string> revMacros = {
		"Room 1",
		"Room 2",
		"Room 3",
		"Hall 1",
		"Hall 2",
		"Plate",
		"Delay",
		"Panning Delay",
	};

	lbReverb_ = new CListBox(processor_, "sysex 40 01 30", revMacros);
	lbReverb_->setRowHeight(25);
	lbReverb_->setBounds(10, 40, 150, 200);

	// Get saved selected row
	int row = (int)* p.getParameters().getRawParameterValue(lbReverb_->getName());
	lbReverb_->selectRow(row);

	addAndMakeVisible(lbReverb_);


	addSlider("Level",		"sysex 40 01 33",	210, 90);
	addSlider("Time",		"sysex 40 01 34",	290, 90);
	addSlider("Feedback",	"sysex 40 01 35",	370, 90);
	addSlider("Pre-LPF",	"sysex 40 01 32",	450, 90);



	addLabel("Chorus", 10, 280, true);

	vector<string> chsMacros = {
		"Chorus 1",
		"Chorus 2",
		"Chorus 3",
		"Chorus 4",
		"Feedback Chorus",
		"Flanger",
		"Short Delay",
		"Short Delay (FB)",
	};

	lbChorus_ = new CListBox(processor_, "sysex 40 01 38", chsMacros);
	lbChorus_->setRowHeight(25);
	lbChorus_->setBounds(10, 310, 150, 200);

	// Get saved selected row
	row = (int)* p.getParameters().getRawParameterValue(lbChorus_->getName());
	lbChorus_->selectRow(row);

	addAndMakeVisible(lbChorus_);


	addSlider("Level",			"sysex 40 01 3a",	210, 310);
	addSlider("Rate",			"sysex 40 01 3d",	290, 310);
	addSlider("Depth",			"sysex 40 01 3e",	370, 310);
	addSlider("Delay",			"sysex 40 01 3c",	450, 310);
	addSlider("Feedback",		"sysex 40 01 3b",	210, 410);
	addSlider("Pre-LPF",		"sysex 40 01 39",	290, 410);
	addSlider("Reverb Send",	"sysex 40 01 3f",	370, 410);
}

MEffects::~MEffects(){

	for(auto i = attachments_.begin(); i != attachments_.end(); i++)
		delete* i;

	for(auto i = sliders_.begin(); i != sliders_.end(); i++)
		delete* i;

	for(auto i = labels_.begin(); i != labels_.end(); i++)
		delete* i;

	delete lbReverb_;
	delete lbChorus_;
}

void MEffects::addSlider(const string& name, string id, int x, int y){

	addLabel(name, x, y + 80, false);

	CSlider* s = new CSlider(processor_, id, x, y);

	sliders_.push_back(s);
	addAndMakeVisible(s);

	attachments_.push_back(new AudioProcessorValueTreeState::SliderAttachment(processor_.getParameters(), id, *s));
}

void MEffects::addLabel(const string& text, int x, int y, bool large){
	Label* l = new Label();
	l->setText(text, dontSendNotification);
	l->setFont(large ? 20 : 15);
	l->setBounds(x, y, large ? 200 : 80, 25);

	if(!large)
		l->setJustificationType(Justification::centred);

	labels_.push_back(l);
	addAndMakeVisible(l);
}
