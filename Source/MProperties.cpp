#include "MProperties.h"

#include"HexFunc.h"
#include<regex>

using std::regex;
using std::smatch;
using std::regex_search;


MProperties::MProperties(RolandControlAudioProcessor& p) : processor_(p), midiOut_(p.getMidiOut()) {

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

	addButton("On/Off", "cc65", 210, 230);

	addSlider("Time", "cc5", 290, 230);


	addLabel("Modulation", 420, 200, true);

	addSlider("Amount", "cc1", 410, 230);

	// not a normal cc controller
	//addSlider("cc71", 90, 490);
	//addLabel("Depth", 100, 500, false);



	addLabel("Effects", 20, 380, true);

	addSlider("Reverb", "cc91", 10, 410);
	addSlider("Chorus", "cc93", 90, 410);


	efxLabel_ = addLabel("EFX", 270, 380, true);

	addSlider("On/Off", "efxDepth", 260, 410, true, true);
	efxParam1Label_ = addSlider("Param1", "efxParam1", 340, 410, true);
	efxParam2Label_ = addSlider("Param2", "efxParam2", 420, 410, true);

	effect_.name = "";
}

MProperties::~MProperties(){

	for(auto i = attachments_.begin(); i != attachments_.end(); i++)
		delete* i;

	for(auto i = bAttachments_.begin(); i != bAttachments_.end(); i++)
		delete* i;

	for(auto i = sliders_.begin(); i != sliders_.end(); i++)
		delete* i;

	for(auto i = buttons_.begin(); i != buttons_.end(); i++)
		delete* i;

	for(auto i = labels_.begin(); i != labels_.end(); i++)
		delete* i;
}

Label* MProperties::addSlider(const string& name, string id, int x, int y, bool efx, bool toggle){

	for(int i = 0; i < 16; i++){

		string sid = id + "ch" + std::to_string(i);
		CSlider* s = new CSlider(processor_, sid, x, y);

		// Special properties for EFX parameter sliders
		if(efx){
			s->addListener(this);
			s->setOnOffDisplay(true);
		}

		sliders_.push_back(s);
		addAndMakeVisible(s);

		attachments_.push_back(new AudioProcessorValueTreeState::SliderAttachment(processor_.getParameters(), sid, *s));

		// Make slider an on/off switch
		if(toggle){
			s->setSliderStyle(Slider::LinearHorizontal);
			s->setRange(s->getMinimum(), 1, 1);

			auto b = s->getBounds();
			b.setX(b.getX() + 15);
			b.setWidth(50);
			s->setBounds(b);
		}
	}

	return addLabel(name, x, y + 80);
}

void MProperties::addButton(const string& name, const string& id, int x, int y){

	addLabel(name, x, y + 60);

	for(int i = 0; i < 16; i++){

		string bid = id + "ch" + std::to_string(i);
		CButton* b = new CButton(processor_, bid, x, y);

		buttons_.push_back(b);
		addAndMakeVisible(b);

		bAttachments_.push_back(new AudioProcessorValueTreeState::ButtonAttachment(processor_.getParameters(), bid, *b));
	}
}

Label* MProperties::addLabel(const string& text, int x, int y, bool large){
	Label* l = new Label();
	l->setText(text, dontSendNotification);
	l->setFont(large ? 20 : 15);
	l->setBounds(x, y, large ? 250 : 80, 25);

	if(!large)
		l->setJustificationType(Justification::centred);

	labels_.push_back(l);
	addAndMakeVisible(l);

	return l;
}

void MProperties::setChannel(int channel){

	for(auto i = sliders_.begin(); i != sliders_.end(); i++){

		// Find "chxx" in ID
		string id = (*i)->getComponentID().toStdString();
		regex rgx("ch(\\d{1,2})");
		smatch matches;
		// Set visible if number in ID is equal to channel
		if(regex_search(id, matches, rgx))
			(*i)->setVisible(channel == std::stoi(matches.str(1)));
	}

	for(auto i = buttons_.begin(); i != buttons_.end(); i++){

		// Find "chxx" in ID
		string id = (*i)->getComponentID().toStdString();
		regex rgx("ch(\\d{1,2})");
		smatch matches;
		// Set visible if number in ID is equal to channel
		if(regex_search(id, matches, rgx))
			(*i)->setVisible(channel == std::stoi(matches.str(1)));
	}
}

void MProperties::setEffect(Effect effect, bool useDefaultValues){

	effect_ = effect;

	// Set labels
	efxLabel_->setText("EFX (" + effect.name + ")", dontSendNotification);

	// Get parameters and set labels
	bool hasParam1 = effect.pparam1 != -1;
	bool hasParam2 = effect.pparam2 != -1;
	EffectParameter param1, param2;

	if(hasParam1){
		param1 = effect.params.at(effect.pparam1);
		efxParam1Label_->setText(param1.name, dontSendNotification);
	}
	else
		efxParam1Label_->setText("Param 1", dontSendNotification);

	if(hasParam2){
		param2 = effect.params.at(effect.pparam2);
		efxParam2Label_->setText(param2.name, dontSendNotification);
	}
	else
		efxParam2Label_->setText("Param 2", dontSendNotification);


	for(auto i = sliders_.begin(); i != sliders_.end(); i++){

		// ID regex
		string id = (*i)->getComponentID().toStdString();
		regex rgx("efxParam(\\d)ch(\\d{1,2})");
		smatch matches;

		if(regex_search(id, matches, rgx)){

			// Set slider properties
			bool hasParam = false;
			EffectParameter param;

			if(std::stoi(matches.str(1)) == 1){
				hasParam = hasParam1;
				param = param1;
			}
			else if(std::stoi(matches.str(1)) == 2){
				hasParam = hasParam2;
				param = param2;
			}

			// Default values for disabled parameter
			if(!hasParam){
				param.type = EffectParameter::SLIDER;
				param.defaultValue = 0;
				param.min = 0;
				param.max = 127;
				param.dispMin = -1;
				param.dispMax = -1;
				param.suffix = "";
			}

			// Always use standard size slider
			param.type = EffectParameter::SLIDER;
			param.width = 80;
			param.height = 80;

			// Set minimum to 1 lower to enable 'off' setting, default to off
			param.min--;
			param.defaultValue = param.min;

			(*i)->setPropertiesFromEffectParameter(param, useDefaultValues);
			(*i)->setEnabled(hasParam);

		}
	}

	// Send EFX parameters
	for(int i = 0; i < 16; i++)
		midiOut_.EFXPartSysex(i, effect_);
}

void MProperties::sliderValueChanged(Slider* slider){

	// ID regex
	string id = slider->getComponentID().toStdString();
	regex rgx("efx.+?ch(\\d{1,2})");
	smatch matches;

	// Send MIDI SysEx out
	if(regex_search(id, matches, rgx) && effect_.name != "")
		midiOut_.EFXPartSysex(std::stoi(matches.str(1)), effect_);
}