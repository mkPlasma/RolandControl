#include"CListBox.h"

#include"HexFunc.h"
#include<regex>

using std::regex;
using std::smatch;
using std::regex_search;


CListBox::CListBox(RolandControlAudioProcessor& p, string name) : ListBox(name), processor_(p), midiOut_(p.getMidiOut()), listener_(nullptr) {
	setModel(this);
	forceRefresh_ = false;

	effect_.name = "";
}

CListBox::CListBox(RolandControlAudioProcessor& p, string name, vector<string> cont) : CListBox(p, name) {
	contents_ = cont;
}

CListBox::~CListBox(){
	//ListBox::~ListBox();
}

int CListBox::getNumRows(){
	// Number of rows must change when contents are refreshed
	return forceRefresh_ ? 0 : contents_.size();
}

void CListBox::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected){
	
	if(rowNumber > contents_.size())
		return;

	if(rowIsSelected)
		g.fillAll(Colour(0xff202024));

	g.setColour(Colours::white);
	g.setFont(height * 0.7);
	g.drawText(contents_[rowNumber], 5, 0, width - 5, height, Justification::centredLeft);
}

void CListBox::setContents(vector<string> cont){
	// Number of rows must change when contents are refreshed
	forceRefresh_ = true;
	updateContent();
	forceRefresh_ = false;

	contents_ = cont;
	updateContent();
}

void CListBox::loadSavedRow(){
	selectRow((int)*processor_.getParameters().getRawParameterValue(getName()));
}

void CListBox::setListener(CListBoxListener* l){
	listener_ = l;
}

void CListBox::selectedRowsChanged(int lastRowSelected){

	// For row change behavior defined elsewhere
	if(listener_ != nullptr){
		listener_->listBoxChanged(this, lastRowSelected);
		return;
	}


	// Save selected
	processor_.getParameters().getParameter(getName())->setValueNotifyingHost(
		lastRowSelected / 127.0f
	);

	// ID regex
	string id = getName().toStdString();
	regex rgx("sysex\\s*([\\da-f]{2}\\s*[\\da-f]{2}\\s*([\\da-f]{2}))");
	smatch matches;

	// Send MIDI SysEx out
	if(regex_search(id, matches, rgx)){
		midiOut_.sysexData((string)matches.str(1), lastRowSelected);

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

void CListBox::setEffect(Effect effect){
	effect_ = effect;
}
