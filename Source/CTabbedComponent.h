#pragma once

#include"../JuceLibraryCode/JuceHeader.h"

const Colour TAB_COLOR_UNSELECTED	= Colour(0.0f, 0.0f, 0.0f, 0.4f);
const Colour TAB_COLOR_SELECTED		= Colours::transparentBlack;

class CTabbedComponent : public TabbedComponent {

	void currentTabChanged(int newCurrentTabIndex, const String& newCurrentTabName) override;
	
public:
	CTabbedComponent(TabbedButtonBar::Orientation orientation);
};