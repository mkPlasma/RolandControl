#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"PluginProcessor.h"
#include"CSlider.h"
#include"CButton.h"
#include"CListBox.h"
#include"EffectList.h"
#include<string>
#include<vector>

using std::string;
using std::vector;

class RolandControlAudioProcessor;
class CSlider;


class MEFX : public Component {

	RolandControlAudioProcessor& processor_;
	Effect effect_;

	vector<CSlider*> sliders_;
	vector<CButton*> buttons_;
	vector<CListBox*> listBoxes_;
	vector<AudioProcessorValueTreeState::SliderAttachment*> attachments_;
	vector<AudioProcessorValueTreeState::ButtonAttachment*> bAttachments_;
	vector<Label*> labels_;

	void reset();

	void addSlider(const EffectParameter& p, const string& id, bool setToDefaultValue);
	void addButton(const EffectParameter& p, const string& id, bool setToDefaultValue);
	void addListBox(const EffectParameter& p, const string& id, bool setToDefaultValue);
	void addLabel(const string& text, int x, int y, bool large = false);

public:
	MEFX(RolandControlAudioProcessor&);
	~MEFX();

	void setEffect(Effect effect, bool useDefaultValues);
};