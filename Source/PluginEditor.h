#pragma once

#include"../JuceLibraryCode/JuceHeader.h"
#include"CLookAndFeel.h"
#include"MProperties.h"
#include"MToneList.h"
#include"MEffects.h"
#include"MEFXSettings.h"
#include"MEFX.h"
#include"PluginProcessor.h"


using std::unique_ptr;

class MProperties;
class MToneList;
class MEffects;
class MEFXSettings;
class MEFX;


//==============================================================================
/**
*/
class RolandControlAudioProcessorEditor  : public AudioProcessorEditor, public TextButton::Listener
{
public:
    RolandControlAudioProcessorEditor(RolandControlAudioProcessor&);
    ~RolandControlAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

	void buttonClicked(Button* btn) override;

	void setEffect(Effect effect, bool useDefaultValues);

	//void setStatus(const string& newStatus, const Colour& colour = Colours::white);

	void noteOn(int channel, int noteNum);
	void noteOff(int channel, int noteNum);

	void setChannelName(int channel, string name);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RolandControlAudioProcessor& processor_;

	CLookAndFeel* clf;

	MProperties*	mProperties_;
	MToneList*		mTonelist_;
	MEffects*		mEffects_;
	MEFXSettings*	mEFXSettings_;
	MEFX*			mEFX_;

	TextButton** channelButtons_;
	TextButton* reloadButton_;

	TabbedComponent* tabs_;

	// Disabled due to crashing
	//Label* status;
	ComponentAnimator* animator_;

	bool note_[16][128];

	// 0 - green (on)
	// 1 - white (on)
	// 2 - black (off)
	// 3 - outline
	Label* noteAnim_[4][16];

	inline void animate(Component* comp, float alpha, int time, double startSpeed);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolandControlAudioProcessorEditor)
};
