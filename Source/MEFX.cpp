#include"MEFX.h"

#include"HexFunc.h"


MEFX::MEFX(RolandControlAudioProcessor& p) : processor_(p) {

}

MEFX::~MEFX(){
	reset();
}

void MEFX::setEffect(Effect effect, bool useDefaultValues){

	effect_ = effect;

	reset();

	// Effect name label
	addLabel(effect.name, 10, 10, true);

	// Add parameters
	for(int i = 0; i < effect.params.size(); i++){

		// Get current parameter
		EffectParameter p = effect.params.at(i);

		// Create ID with proper address
		char id[32];
		unsigned char c = i + 3;
		sprintf(id, "sysex 40 03 %s", byteToHex(c).c_str());

		// Create parameter component
		switch(p.type){

		case EffectParameter::SLIDER:
		case EffectParameter::SLIDER_H:
		case EffectParameter::SLIDER_V:
			addSlider(p, id, useDefaultValues);
			break;

		case EffectParameter::BUTTON:
			addButton(p, id, useDefaultValues);
			break;

		case EffectParameter::LISTBOX:
			addListBox(p, id, useDefaultValues);
			break;
		}
	}
}

void MEFX::reset(){

	for(auto i = attachments_.begin(); i != attachments_.end(); i++)
		delete* i;

	for(auto i = bAttachments_.begin(); i != bAttachments_.end(); i++)
		delete* i;

	for(auto i = sliders_.begin(); i != sliders_.end(); i++)
		delete* i;

	for(auto i = buttons_.begin(); i != buttons_.end(); i++)
		delete* i;

	for(auto i = listBoxes_.begin(); i != listBoxes_.end(); i++)
		delete* i;

	for(auto i = labels_.begin(); i != labels_.end(); i++)
		delete* i;

	attachments_.clear();
	bAttachments_.clear();
	sliders_.clear();
	buttons_.clear();
	listBoxes_.clear();
	labels_.clear();
}

void MEFX::addSlider(const EffectParameter& p, const string& id, bool setToDefaultValue){

	addLabel(p.name, p.x, p.y + p.height, false);

	CSlider* s = new CSlider(processor_, id, p.x, p.y);
	s->setEffect(effect_);

	sliders_.push_back(s);
	addAndMakeVisible(s);

	attachments_.push_back(new AudioProcessorValueTreeState::SliderAttachment(processor_.getParameters(), id, *s));

	// Set properties after adding attachment
	s->setPropertiesFromEffectParameter(p, setToDefaultValue);

	// Force MIDI output
	if(s->getValue() == 0)
		s->sliderValueChanged(s);
}

void MEFX::addButton(const EffectParameter& p, const string& id, bool setToDefaultValue){

	addLabel(p.name, p.x, p.y + 60, false);

	CButton* b = new CButton(processor_, id, p.x, p.y);
	b->setEffect(effect_);

	buttons_.push_back(b);
	addAndMakeVisible(b);

	bAttachments_.push_back(new AudioProcessorValueTreeState::ButtonAttachment(processor_.getParameters(), id, *b));

	if(setToDefaultValue)
		b->setToggleState(p.defaultValue == 1, dontSendNotification);

	// Force MIDI output
	if(!b->getToggleState())
		b->buttonClicked(b);
}

void MEFX::addListBox(const EffectParameter& p, const string& id, bool setToDefaultValue){

	addLabel(p.name, p.x, p.y - 30, true);

	CListBox* lb = new CListBox(processor_, id, p.values);
	lb->setRowHeight(25);
	lb->setEffect(effect_);

	// Set height to 0 to be sized to fit list
	lb->setBounds(p.x, p.y, p.width, p.height == 0 ? p.values.size()*25 : p.height);

	if(setToDefaultValue)
		lb->selectRow(p.defaultValue);
	else
		lb->loadSavedRow();

	listBoxes_.push_back(lb);
	addAndMakeVisible(lb);
}


void MEFX::addLabel(const string& text, int x, int y, bool large){
	Label* l = new Label();
	l->setText(text, dontSendNotification);
	l->setFont(large ? 20 : 15);
	l->setBounds(x, y, large ? 200 : 80, 25);

	if(!large)
		l->setJustificationType(Justification::centred);

	labels_.push_back(l);
	addAndMakeVisible(l);
}
