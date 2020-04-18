#include "CSlider.h"

#include<regex>
#include"HexFunc.h"

using std::regex;
using std::smatch;
using std::regex_search;


CSlider::CSlider(RolandControlAudioProcessor& p, const string& id, int x, int y) : Slider(), processor_(p), midiOut_(p.getMidiOut()) {
	setComponentID(id);
	addListener(this);

	setSliderStyle(RotaryHorizontalVerticalDrag);
	setTextBoxStyle(TextBoxBelow, false, 50, 20);

	setBounds(x, y, 80, 80);

	dispMin_ = -1;
	dispMax_ = -1;
	dispOnOff_ = false;

	effect_.name = "";
}

void CSlider::setPropertiesFromEffectParameter(EffectParameter p, bool setToDefaultValue){

	switch(p.type){
	case EffectParameter::SLIDER_H:	setSliderStyle(Slider::LinearHorizontal);	break;
	case EffectParameter::SLIDER_V:	setSliderStyle(Slider::LinearVertical);		break;
	}

	setSize(p.width, p.height);

	setRange(p.min, p.max, 1);
	setTextValueSuffix(p.suffix);

	setDisplayRange(p.dispMin, p.dispMax);
	setDispValueList(p.values);

	if(setToDefaultValue)
		setValue(p.defaultValue);
	setDoubleClickReturnValue(true, p.defaultValue);


	updateText();
}

void CSlider::setEffect(Effect effect){
	effect_ = effect;
}

void CSlider::setDisplayRange(double dispMin, double dispMax){
	dispMin_ = dispMin;
	dispMax_ = dispMax;
}

void CSlider::setDispValueList(vector<string> dispValueList){
	dispValueList_ = dispValueList;
}

void CSlider::setOnOffDisplay(bool dispOnOff){
	dispOnOff_ = dispOnOff;
}

String CSlider::getTextFromValue(double v){

	if(dispOnOff_){
		// Display 'Off' instead of minimum value
		if(getValue() == getMinimum())
			return "Off";

		// Display 'On' if max is 1
		if(getValue() == getMaximum() && getMaximum() == 1)
			return "On";
	}

	// Normal display values
	if((dispMin_ == -1 || dispMax_ == -1) && dispValueList_.empty())
		return Slider::getTextFromValue(v);


	// Adjust minimum value for 'off' display
	double min = getMinimum();

	if(dispOnOff_)
		min++;

	// Custom min/max
	if(dispMin_ != -1){
		double value = dispMin_ + ((getValue() - min) / (getMaximum() - min)) * (dispMax_ - dispMin_);
		bool round = dispMin_ == (int)dispMin_ && dispMax_ == (int)dispMax_;

		return (round ? String(roundToInt(value)) : String(value, 2)) + getTextValueSuffix();
	}

	// Custom values list
	return String(dispValueList_.at(getValue() - min)) + getTextValueSuffix();
}

double CSlider::getValueFromText(const String& text){

	auto t = text.trim();

	// Remove suffix if present
	if(t.endsWith(getTextValueSuffix()))
		t = t.substring(0, t.length() - getTextValueSuffix().length());

	// Remove plus sign if present
	while(t.startsWithChar('+'))
		t = t.substring(1).trimStart();


	if(dispOnOff_){
		// Set to minimum if 'off' is typed
		if(t.equalsIgnoreCase("off"))
			return getMinimum();

		// Set to 1 if 'on' is typed and max is 1
		if(t.equalsIgnoreCase("on") && getMaximum() == 1)
			return 1;
	}


	// Item in list
	if(!dispValueList_.empty()){

		// Check for match first
		for(int i = 0; i < dispValueList_.size(); i++)
			if(t.equalsIgnoreCase(String(dispValueList_.at(i))))
				return i;

		// Check for partial match
		for(int i = 0; i < dispValueList_.size(); i++)
			if(String(dispValueList_.at(i)).startsWithIgnoreCase(t))
				return i;

		// Default to no change
		return getValue();
	}

	// Normal value
	double val = t.initialSectionContainingOnly("0123456789.,-").getDoubleValue();

	if((dispMin_ == -1 || dispMax_ == -1))
		return val;

	// Adjust minimum value for 'off' display
	double min = getMinimum();

	if(dispOnOff_)
		min++;

	return min + ((val - dispMin_) / (dispMax_ - dispMin_)) * (getMaximum() - min);
}

void CSlider::sliderValueChanged(Slider* slider){

	// ID regex
	string id = slider->getComponentID().toStdString();
	smatch matches;

	// Send MIDI CC out
	regex rgx("cc(\\d{1,2})ch(\\d{1,2})");
	if(regex_search(id, matches, rgx)){

		int val = slider->getValue();

		// Adjust value for parameters that can be negative
		val += processor_.getParameters().getParameter(id)->getNormalisableRange().getRange().getStart() < 0 ? 64 : 0;

		midiOut_.cc(std::stoi(matches.str(2)), std::stoi(matches.str(1)), val);
		return;
	}

	// Send MIDI SysEx out
	rgx = regex("sysex\\s*([\\da-f]{2}\\s*[\\da-f]{2}\\s*([\\da-f]{2}))");
	if(regex_search(id, matches, rgx)){
		midiOut_.sysexData((string)matches.str(1), slider->getValue());

		// Resend MIDI for any overridden EFX parameters
		int currentParam = hexStringToByte((string)matches.str(2)) - 3;
		bool matchesP1 = currentParam == effect_.pparam1;
		bool matchesP2 = currentParam == effect_.pparam2;

		if(effect_.name != "" && (matchesP1 || matchesP2)){

			for(int i = 0; i < 16; i++){
				int depth = (int)*processor_.getParameters().getRawParameterValue("efxDepthch" + std::to_string(i));
				int param1 = (int)*processor_.getParameters().getRawParameterValue("efxParam1ch" + std::to_string(i));
				int param2 = (int)*processor_.getParameters().getRawParameterValue("efxParam2ch" + std::to_string(i));
				int min1 = effect_.params[effect_.pparam1].min - 1;
				int min2 = effect_.params[effect_.pparam1].min - 1;

				if(depth != 0 && ((matchesP1 && param1 != min1) || (matchesP2 && param2 != min2)))
					midiOut_.EFXPartSysex(i, effect_);
			}
		}
	}
}