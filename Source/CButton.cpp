#include"CButton.h"

#include"HexFunc.h"
#include<regex>

using std::regex;
using std::smatch;
using std::regex_search;

CButton::CButton(RolandControlAudioProcessor& p, const string& id, int x, int y) : ToggleButton(), processor_(p), midiOut_(p.getMidiOut()) {
	setComponentID(id);
	addListener(this);

	// Correct button placement
	setBounds(x + 28, y - 10, 80, 80);

	effect_.name = "";
}

void CButton::setEffect(Effect effect){
	effect_ = effect;
}

void CButton::buttonClicked(Button* button){

	// ID regex
	string id = button->getComponentID().toStdString();
	smatch matches;

	// Send MIDI CC out
	regex rgx("cc(\\d{1,2})ch(\\d{1,2})");
	if(regex_search(id, matches, rgx)){
		midiOut_.cc(std::stoi(matches.str(2)), std::stoi(matches.str(1)), button->getToggleState() ? 127 : 0);
		return;
	}

	// Send MIDI SysEx out
	rgx = regex("sysex\\s*([\\da-f]{2}\\s*[\\da-f]{2}\\s*([\\da-f]{2}))");
	if(regex_search(id, matches, rgx)){
		midiOut_.sysexData((string)matches.str(1), button->getToggleState() ? 1 : 0);

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