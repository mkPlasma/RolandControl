#include "MToneList.h"


MToneList::MToneList(RolandControlAudioProcessor& p, RolandControlAudioProcessorEditor& e) : processor_(p), editor_(e), midiOut_(p.getMidiOut()), toneList_(p.getToneList()) {

	channel_ = 0;

	// Create tone listBox
	lbTones_ = new CListBox(processor_, "tone" + std::to_string(channel_));
	lbTones_->setListener(this);
	lbTones_->setRowHeight(22);
	lbTones_->setBounds(220, 10, 370, 510);

	addAndMakeVisible(lbTones_);


	// Create category listBox
	lbCategories_ = new CListBox(processor_, "cat" + std::to_string(channel_), toneList_.getCategoryNames());
	lbCategories_->setListener(this);
	lbCategories_->setRowHeight(510 / toneList_.getTones().size());
	lbCategories_->setBounds(10, 10, 200, 510);

	// Load channel tones
	setTones();

	// Set to default channel
	setChannel(0);

	addAndMakeVisible(lbCategories_);
}

MToneList::~MToneList(){
	delete lbCategories_;
	delete lbTones_;
}

void MToneList::refresh(){

	// Unset tone if categories removed on reload
	int max = toneList_.getTones().size() - 1;

	for(int i = 0; i < 16; i++){
		int n = (int)*processor_.getParameters().getRawParameterValue("cat" + std::to_string(i));
		
		if(n > max)
			lbTones_->selectRow(max - 1);
	}

	// Fix tone if tones were removed on reload
	for(int i = 0; i < 16; i++){
		int n = (int)*processor_.getParameters().getRawParameterValue("tone" + std::to_string(i));
		max = toneList_.getTones()[lbCategories_->getLastRowSelected()].tones.size() - 1;

		if(n > max)
			lbTones_->selectRow(max);
	}

	// Refresh categories
	lbCategories_->setContents(toneList_.getCategoryNames());
	lbCategories_->setRowHeight(510 / toneList_.getTones().size());

	// Select proper category
	setChannel(channel_);
}

void MToneList::setTones(){
	for(int i = 0; i < 16; i++){

		// If tone is set, send MIDI and set proper name
		if((bool)*processor_.getParameters().getRawParameterValue("toneSet" + std::to_string(i))){
			Tone tone = toneList_.getTones()[
				(int)*processor_.getParameters().getRawParameterValue("cat" + std::to_string(i))
			].tones[(int)*processor_.getParameters().getRawParameterValue("tone" + std::to_string(i))];

				midiOut_.setTone(i, tone.pc, tone.msb, tone.lsb);
				editor_.setChannelName(i, tone.name);
		}
		else
			editor_.setChannelName(i, "-");
	}
}

void MToneList::setChannel(int channel){
	channel_ = channel;

	// Get saved selected row
	//int row = (int)*processor_.getParameters().getRawParameterValue("catSel" + std::to_string(channel_));
	int row = (int)*processor_.getParameters().getRawParameterValue("cat" + std::to_string(channel_));

	// If not yet set, set to last row (blank instrument)
	row = (bool)*processor_.getParameters().getRawParameterValue("toneSet" + std::to_string(channel_)) ? row : toneList_.getTones().size() - 1;

	lbCategories_->selectRow(row);

	// Reload to make sure correct tone is selected
	listBoxChanged(lbCategories_, row);
}

void MToneList::listBoxChanged(ListBox* clb, int row){

	// Ignore deselected row
	if(row == -1)
		return;

	// Category changed
	if(clb == lbCategories_){
		lbTones_->setContents(toneList_.getToneNames(row));

		// Save selected category
		/*
		processor_.getParameters().getParameter("catSel" + std::to_string(channel_))->setValueNotifyingHost(
			row / 127.0f
		);
		*/
			
		// If last category (blank instrument) is selected, select first tone
		if(row == toneList_.getTones().size() - 1){
			lbTones_->selectRow(0);
			editor_.setChannelName(channel_, toneList_.getTones()[row].tones[0].name);
			processor_.getParameters().getParameter("toneSet" + std::to_string(channel_))->setValueNotifyingHost(false);
			return;
		}

		// If selected tone is in this category, highlight it
		if(row == (int)*processor_.getParameters().getRawParameterValue("cat" + std::to_string(channel_)) &&
			(bool)*processor_.getParameters().getRawParameterValue("toneSet" + std::to_string(channel_))){
			lbTones_->selectRow((int)*processor_.getParameters().getRawParameterValue("tone" + std::to_string(channel_)));
			return;
		}
		
		lbTones_->deselectAllRows();
		return;
	}

	// Tone changed and is not in blank category
	if(clb == lbTones_ && lbCategories_->getLastRowSelected() != toneList_.getTones().size() - 1){

		Tone tone = toneList_.getTones()[lbCategories_->getLastRowSelected()].tones[row];

		midiOut_.setTone(channel_, tone.pc, tone.msb, tone.lsb);

		// Set channel button text
		editor_.setChannelName(channel_, tone.name);

		// Save category and tone
		processor_.getParameters().getParameter("cat" + std::to_string(channel_))->setValueNotifyingHost(
			lbCategories_->getLastRowSelected() / 127.0f
		);
		processor_.getParameters().getParameter("tone" + std::to_string(channel_))->setValueNotifyingHost(
			row / 127.0f
		);
		processor_.getParameters().getParameter("toneSet" + std::to_string(channel_))->setValueNotifyingHost(true);

		return;
	}
}