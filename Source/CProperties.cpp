#include "CProperties.h"
#include "CSlider.h"
#include<regex>

using std::regex;
using std::smatch;
using std::regex_search;


CProperties::CProperties(RolandControlAudioProcessor& p) : processor_(p), midiOut(p.getMidiOut()) {

	addLabel("Envelope", 20, 20, true);

	addSlider("Attack", "cc73", 10, 50);
	addSlider("Decay", "cc75", 90, 50);
	addSlider("Release", "cc72", 170, 50);


	addLabel("Vibrato", 350, 20, true);

	addSlider("Depth", "cc77", 340, 50);
	addSlider("Rate", "cc76", 420, 50);
	addSlider("Delay", "cc78", 500, 50);



	addLabel("Filter", 20, 200, true);

	addSlider("Cutoff", "cc74", 10, 230);
	addSlider("Resonance", "cc71", 90, 230);


	addLabel("Portamento", 220, 200, true);

	addButton("cc65", 238, 230);
	addLabel("On/Off", 210, 290, false);

	addSlider("Time", "cc5", 290, 230);


	addLabel("Modulation", 420, 200, true);

	addSlider("Amount", "cc1", 410, 230);

	// not a normal cc controller
	//addSlider("cc71", 90, 490);
	//addLabel("Depth", 100, 500, false);



	addLabel("Effects", 20, 380, true);

	addSlider("Reverb", "cc91", 10, 410);
	addSlider("Chorus", "cc93", 90, 410);

}

CProperties::~CProperties(){

	for(auto i = attachments.begin(); i != attachments.end(); i++)
		delete* i;

	for(auto i = bAttachments.begin(); i != bAttachments.end(); i++)
		delete* i;

	for(auto i = sliders.begin(); i != sliders.end(); i++)
		delete* i;

	for(auto i = buttons.begin(); i != buttons.end(); i++)
		delete* i;

	for(auto i = labels.begin(); i != labels.end(); i++)
		delete* i;
}

void CProperties::addSlider(string name, string id, int x, int y){

	addLabel(name, x, y + 80);

	for(int i = 0; i < 16; i++){
		string sid = id + "ch" + std::to_string(i);
		CSlider* s = new CSlider(sid, x, y);

		sliders.push_back(s);
		addAndMakeVisible(s);

		attachments.push_back(new AudioProcessorValueTreeState::SliderAttachment(processor_.getParameters(), sid, *s));
	}
}

void CProperties::addButton(string id, int x, int y){

	for(int i = 0; i < 16; i++){

		ToggleButton* b = new ToggleButton();
		string bid = id + "ch" + std::to_string(i);

		b->setComponentID(bid);
		b->addListener(this);

		b->setBounds(x, y, 80, 80);

		buttons.push_back(b);
		addAndMakeVisible(b);

		bAttachments.push_back(new AudioProcessorValueTreeState::ButtonAttachment(processor_.getParameters(), bid, *b));
	}
}

void CProperties::addLabel(string text, int x, int y, bool large){
	Label* l = new Label();
	l->setText(text, dontSendNotification);
	l->setFont(large ? 20 : 15);
	l->setBounds(x, y, large ? 200 : 80, 25);

	if(!large)
		l->setJustificationType(Justification::centred);

	labels.push_back(l);
	addAndMakeVisible(l);
}

void CProperties::setChannel(int channel){

	for(auto i = sliders.begin(); i != sliders.end(); i++){

		// Find "chxx" in ID
		string id = (*i)->getComponentID().toStdString();
		regex rgx("ch(\\d{1,2})");
		smatch matches;
		// Set visible if number in ID is equal to channel
		if(regex_search(id, matches, rgx))
			(*i)->setVisible(channel == std::stoi(matches.str(1)));
	}

	for(auto i = buttons.begin(); i != buttons.end(); i++){

		// Find "chxx" in ID
		string id = (*i)->getComponentID().toStdString();
		regex rgx("ch(\\d{1,2})");
		smatch matches;
		// Set visible if number in ID is equal to channel
		if(regex_search(id, matches, rgx))
			(*i)->setVisible(channel == std::stoi(matches.str(1)));
	}
}

void CProperties::buttonClicked(Button* btn){

	// ID regex
	string id = btn->getComponentID().toStdString();
	regex rgx("cc(\\d{1,2})ch(\\d{1,2})");
	smatch matches;

	// Send MIDI cc out
	if(regex_search(id, matches, rgx))
		midiOut->cc(std::stoi(matches.str(2)), std::stoi(matches.str(1)), btn->getToggleState() ? 127 : 0);


	// temp efx test remove later

	// set part efx parameters
	char data[] = {
		// efx type msb, lsb
		0x01, 0x14,

		0x00, 0x7f, 0x00, 0x00
	};
	midiOut->sysexData("40 41 23", data, sizeof(data));



	// set efx type and mix level
	char data2[] = {0x01, 0x14};
	midiOut->sysexData("40 03 00", data2, sizeof(data2));
	midiOut->sysexData("40 03 1a", 0x7f);

	// efx parameters
	midiOut->sysexData("40 03 03", 0x01);
	midiOut->sysexData("40 03 04", 0x00);
	midiOut->sysexData("40 03 05", 0x7f);
	midiOut->sysexData("40 03 06", 0x7f);
	midiOut->sysexData("40 03 07", 0x01);
	midiOut->sysexData("40 03 08", 0x50);
	midiOut->sysexData("40 03 09", 0x50);
	midiOut->sysexData("40 03 0a", 0x50);
	midiOut->sysexData("40 03 0b", 0x50);
	midiOut->sysexData("40 03 0c", 0x00);
	midiOut->sysexData("40 03 0d", 0x01);
	midiOut->sysexData("40 03 0e", 0x00);
	midiOut->sysexData("40 03 0f", 0x00);
	midiOut->sysexData("40 03 10", 0x7f);
	midiOut->sysexData("40 03 11", 0x7f);
	midiOut->sysexData("40 03 12", 0x40);
	midiOut->sysexData("40 03 13", 0x50);
}