#include "PluginEditor.h"
#include "CTabbedComponent.h"

//==============================================================================
RolandControlAudioProcessorEditor::RolandControlAudioProcessorEditor (RolandControlAudioProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {

    setSize(970, 600);

	clf = new CLookAndFeel();
	//setLookAndFeel(clf);

	// Init channel buttons
	channelButtons_ = new TextButton*[16];

	// Set selected channel
	int ch = (int)*p.getParameters().getRawParameterValue("channel");

	for(int i = 0; i < 16; i++){

		TextButton* btn = new TextButton((i < 9 ? "0" : "") + std::to_string(i + 1));
		btn->setLookAndFeel(clf);

		btn->addListener(this);
		btn->setClickingTogglesState(true);
		btn->setRadioGroupId(1001);
		btn->setBounds(20, 20 + i*35, 320, 35);
		addAndMakeVisible(btn);

		if(i == ch)
			btn->setToggleState(true, 0);

		channelButtons_[i] = btn;
	}


	// Init top tabs
	tabs_ = new CTabbedComponent(TabbedButtonBar::TabsAtTop);
	tabs_->setTabBarDepth(30);
	tabs_->setCurrentTabIndex(0);
	tabs_->setBounds(350, 20, 600, 560);
	
	mProperties_ = new MProperties(p);
	mProperties_->setChannel(ch);
	
	mTonelist_ = new MToneList(p, *this);
	mTonelist_->setChannel(ch);
	
	mEffects_ = new MEffects(p);

	mEFX_ = new MEFX(p);
	mEFXSettings_ = new MEFXSettings(p, *this);
	
	tabs_->addTab("Properties",		Colours::transparentBlack, mProperties_,	false);
	tabs_->addTab("Tone List",		Colours::transparentBlack, mTonelist_,		false);
	tabs_->addTab("Effects",		Colours::transparentBlack, mEffects_,		false);
	tabs_->addTab("EFX Settings",	Colours::transparentBlack, mEFXSettings_,	false);
	tabs_->addTab("EFX",			Colours::transparentBlack, mEFX_,			false);
	
	tabs_->currentTabChanged(0, "");
	addAndMakeVisible(tabs_);

	reloadButton_ = new TextButton("Reload files");
	reloadButton_->addListener(this);
	reloadButton_->setBounds(870, 20, 80, 25);
	addAndMakeVisible(reloadButton_);

	animator_ = new ComponentAnimator();

	// Disabled due to crashing
	/*
	status = new Label("status", "");
	status->setBounds(25, 10, 400, 25);
	addAndMakeVisible(status);

	MidiOut* midiOut = p.getMidiOut();

	if(midiOut->open())
		setStatus("MIDI out opened sucessfully!", Colours::lime);
	else
		setStatus("MIDI out open failed!", Colours::red);
	*/

	for(int i = 0; i < 16; i++){

		for(int j = 0; j < 128; j++)
			note_[i][j] = false;

		// Green (on)
		noteAnim_[0][i] = new Label("", "");
		noteAnim_[0][i]->setBounds(292, 32 + i * 35, 26, 11);
		noteAnim_[0][i]->setColour(Label::backgroundColourId, Colours::lime);
		noteAnim_[0][i]->setAlpha(0);

		// White (on)
		noteAnim_[1][i] = new Label("", "");
		noteAnim_[1][i]->setBounds(292, 32 + i * 35, 26, 11);
		noteAnim_[1][i]->setColour(Label::backgroundColourId, Colours::white);
		noteAnim_[1][i]->setAlpha(0);

		// Black (off)
		noteAnim_[2][i] = new Label("", "");
		noteAnim_[2][i]->setBounds(292, 32 + i * 35, 26, 11);
		noteAnim_[2][i]->setColour(Label::backgroundColourId, Colours::black);

		// Outline
		noteAnim_[3][i] = new Label("", "");
		noteAnim_[3][i]->setBounds(290, 30 + i * 35, 30, 15);
		noteAnim_[3][i]->setColour(Label::backgroundColourId, Colours::darkgrey);

		// Add bottom-top order
		addAndMakeVisible(noteAnim_[3][i]);
		addAndMakeVisible(noteAnim_[2][i]);
		addAndMakeVisible(noteAnim_[0][i]);
		addAndMakeVisible(noteAnim_[1][i]);
	}
}

RolandControlAudioProcessorEditor::~RolandControlAudioProcessorEditor(){

	for(int i = 0; i < 16; i++)
		delete channelButtons_[i];

	delete tabs_;
	delete reloadButton_;

	delete mProperties_;
	delete mTonelist_;
	delete mEffects_;
	delete mEFXSettings_;
	delete mEFX_;
	
	//delete status;
	delete animator_;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 16; j++)
			delete noteAnim_[i][j];
	
	setLookAndFeel(nullptr);
	delete clf;
}


//==============================================================================
void RolandControlAudioProcessorEditor::paint(Graphics& g){

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));


	// Draw status bar
	// Disabled due to crashing
	/*
	const juce::Rectangle<int> STATUS_RECT = {20, 10, 400, 25};

	g.setColour(Colours::black);
	g.fillRect(STATUS_RECT);

	g.setColour(Colours::white);
	g.drawRect(STATUS_RECT, 1);
	*/
}

void RolandControlAudioProcessorEditor::resized(){
}

void RolandControlAudioProcessorEditor::buttonClicked(Button* btn){

	// Reload button
	if(btn == reloadButton_){
		processor_.getToneList().reload();
		processor_.getEffectList().reload();

		mTonelist_->refresh();
		mEFXSettings_->refresh();
		return;
	}


	// Channel buttons

	// Check if button is on
	if(!btn->getToggleState())
		return;

	// Find button clicked
	for(int i = 0; i < 16; i++){
		
		if(channelButtons_[i] == btn){
			// Save selected channel
			processor_.getParameters().getParameter("channel")->setValueNotifyingHost(i/16.0f);

			mProperties_->setChannel(i);
			mTonelist_->setChannel(i);
		}
	}
}

//==============================================================================

void RolandControlAudioProcessorEditor::setEffect(Effect effect, bool useDefaultValues){
	if(mProperties_ != nullptr)
		mProperties_->setEffect(effect, useDefaultValues);
	if(mEFX_ != nullptr)
		mEFX_->setEffect(effect, useDefaultValues);
}

// Disabled due to crashing
/*
void RolandControlAudioProcessorEditor::setStatus(const string& newStatus, const Colour& colour){
	status->setText(newStatus, dontSendNotification);
	status->setColour(Label::textColourId, colour);
	status->setAlpha(1);
	animate(status, 0, 8000, 0);
}
*/

void RolandControlAudioProcessorEditor::noteOn(int channel, int noteNum){

	note_[channel][noteNum] = true;
	
	noteAnim_[0][channel]->setAlpha(1);
	noteAnim_[1][channel]->setAlpha(1);
	animate(noteAnim_[0][channel], 0.75f, 200, 0.5);
	animate(noteAnim_[1][channel], 0.25f, 125, 0.5);
}

void RolandControlAudioProcessorEditor::noteOff(int channel, int noteNum){

	note_[channel][noteNum] = false;

	// Check all notes off
	for(int i = 0; i < 128; i++)
		if(note_[channel][i])
			return;

	animate(noteAnim_[0][channel], 0, 200, 0.5);
	animate(noteAnim_[1][channel], 0, 125, 0.5);
}

void RolandControlAudioProcessorEditor::animate(Component* comp, float alpha, int time, double startSpeed){
	animator_->animateComponent(comp, comp->getBounds(), alpha, time, false, startSpeed, 1);
}

void RolandControlAudioProcessorEditor::setChannelName(int channel, string name){
	channelButtons_[channel]->setButtonText((channel < 9 ? "0" : "") + std::to_string(channel + 1) + name);
}