#include"MEFXSettings.h"

#include"HexFunc.h"


MEFXSettings::MEFXSettings(RolandControlAudioProcessor& p, RolandControlAudioProcessorEditor& e) : processor_(p), editor_(e), midiOut_(p.getMidiOut()), effectList_(p.getEffectList()) {

	// Depth, Reverb and Chorus send
	addSlider("EFX Depth", "sysex 40 03 1a", 10, 410);
	addSlider("Reverb Send", "sysex 40 03 17", 110, 410);
	addSlider("Chorus Send", "sysex 40 03 18", 210, 410);


	// Load saved effect to determine whether to use default parameter values
	if((bool)*p.getParameters().getRawParameterValue("efxSet")){
		int cat = (int)*p.getParameters().getRawParameterValue("efxCat");
		int efx = (int)*p.getParameters().getRawParameterValue("efx");

		if(cat >= effectList_.getEffects().size() || efx >= effectList_.getEffects()[cat].effects.size())
			prevEffect_.name = "";
		else
			prevEffect_ = effectList_.getEffects()[cat].effects[efx];
	}
	else
		prevEffect_.name = "";

	// Create effects listBox
	lbEffects_ = new CListBox(processor_, "efx");
	lbEffects_->setListener(this);
	lbEffects_->setRowHeight(24);
	lbEffects_->setBounds(220, 10, 370, 390);

	addAndMakeVisible(lbEffects_);


	// Create category listBox
	lbCategories_ = new CListBox(processor_, "efxCat", effectList_.getCategoryNames());
	lbCategories_->setListener(this);
	lbCategories_->setRowHeight(26);
	lbCategories_->setBounds(10, 10, 200, 390);

	// Get saved selected row
	//lbCategories_->selectRow((int)* p.getParameters().getRawParameterValue("efxCatSel"));
	lbCategories_->selectRow((int)* p.getParameters().getRawParameterValue("efxCat"));

	addAndMakeVisible(lbCategories_);


	// Allow values to be saved after initial setting to default values
	processor_.getParameters().getParameter("efxSet")->setValueNotifyingHost(true);
}

MEFXSettings::~MEFXSettings(){

	delete lbCategories_;
	delete lbEffects_;

	for(auto i = attachments_.begin(); i != attachments_.end(); i++)
		delete* i;

	for(auto i = sliders_.begin(); i != sliders_.end(); i++)
		delete* i;

	for(auto i = labels_.begin(); i != labels_.end(); i++)
		delete* i;
}

void MEFXSettings::refresh(){

	// Refresh categories
	lbCategories_->setContents(effectList_.getCategoryNames());

	// Get saved row
	int row = (int)*processor_.getParameters().getRawParameterValue("efxCat");

	// Fix if rows were removed on reload
	int max = effectList_.getEffects().size() - 1;
	row = row > max ? max : row;
	lbCategories_->selectRow(row);


	// Refresh tones
	lbEffects_->setContents(effectList_.getEffectNames(lbCategories_->getLastRowSelected()));

	// Get saved row
	row = (int)*processor_.getParameters().getRawParameterValue("efx");

	// Fix if rows were removed on reload
	max = effectList_.getEffects()[lbCategories_->getLastRowSelected()].effects.size() - 1;
	row = row > max ? max : row;
	lbEffects_->selectRow(row);
}

void MEFXSettings::addSlider(const string& name, string id, int x, int y){

	addLabel(name, x, y + 80, false);

	CSlider* s = new CSlider(processor_, id, x, y);

	sliders_.push_back(s);
	addAndMakeVisible(s);

	attachments_.push_back(new AudioProcessorValueTreeState::SliderAttachment(processor_.getParameters(), id, *s));
}

void MEFXSettings::addLabel(const string& text, int x, int y, bool large){
	Label* l = new Label();
	l->setText(text, dontSendNotification);
	l->setFont(large ? 20 : 15);
	l->setBounds(x, y, large ? 200 : 80, 25);

	if(!large)
		l->setJustificationType(Justification::centred);

	labels_.push_back(l);
	addAndMakeVisible(l);
}


void MEFXSettings::listBoxChanged(ListBox* clb, int row){

	// Ignore deselected row
	if(row == -1)
		return;


	// Category changed
	if(clb == lbCategories_){
		lbEffects_->setContents(effectList_.getEffectNames(row));

		// Save selected category
		/*
		processor_.getParameters().getParameter("efxCatSel")->setValueNotifyingHost(
			row / 127.0f
		);
		*/

		// If selected effect is in this category, highlight it
		if(row == (int)*processor_.getParameters().getRawParameterValue("efxCat")){
			lbEffects_->selectRow((int)*processor_.getParameters().getRawParameterValue("efx"));
			return;
		}

		lbEffects_->deselectAllRows();
		return;
	}

	// Effect changed
	if(clb == lbEffects_){
		int cat = lbCategories_->getLastRowSelected();

		// If the selected effect is out of range, select the first one instead
		if(cat >= effectList_.getEffects().size() || row >= effectList_.getEffects()[cat].effects.size())
			effect_ = effectList_.getEffects()[0].effects[0];
		else
			effect_ = effectList_.getEffects()[cat].effects[row];

		bool changed = effect_.name != prevEffect_.name;

		// Only send MIDI if effect was changed
		if(changed){
			// Set EFX type
			char data[] = {effect_.msb, effect_.lsb};
			midiOut_.sysexData("40 03 00", data, sizeof(data));

			// Set EFX depth and send levels
			midiOut_.sysexData("40 03 1a", (int)*processor_.getParameters().getRawParameterValue("sysex 40 03 1a"));
			midiOut_.sysexData("40 03 17", (int)*processor_.getParameters().getRawParameterValue("sysex 40 03 17"));
			midiOut_.sysexData("40 03 18", (int)*processor_.getParameters().getRawParameterValue("sysex 40 03 18"));
		}

		// Set MFX menu effect, using default values if effect was changed
		editor_.setEffect(effect_, changed);


		// Save category and effect
		processor_.getParameters().getParameter("efxCat")->setValueNotifyingHost(
			lbCategories_->getLastRowSelected() / 127.0f
		);
		processor_.getParameters().getParameter("efx")->setValueNotifyingHost(
			row / 127.0f
		);

		prevEffect_ = effect_;
		return;
	}
}