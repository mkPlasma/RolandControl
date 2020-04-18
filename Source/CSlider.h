#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"PluginProcessor.h"
#include"EffectList.h"
#include<string>
#include<vector>

using std::string;
using std::vector;

class RolandControlAudioProcessor;
class MidiOut;


class CSlider : public Slider, public Slider::Listener {

	RolandControlAudioProcessor& processor_;
	MidiOut& midiOut_;

	double dispMin_, dispMax_;
	vector<string> dispValueList_;
	bool dispOnOff_;

	Effect effect_;

public:
	CSlider(RolandControlAudioProcessor& p, const string& id, int x, int y);

	void setPropertiesFromEffectParameter(EffectParameter p, bool setToDefaultValue);
	void setEffect(Effect effect);

	void setDisplayRange(double dispMin, double dispMax);
	void setDispValueList(vector<string> dispValueList);
	void setOnOffDisplay(bool dispOff);

	String getTextFromValue(double v) override;
	double getValueFromText(const String& text) override;

	void sliderValueChanged(Slider* slider) override;
};