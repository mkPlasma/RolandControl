#pragma once

#include"../JuceLibraryCode/JuceHeader.h"


class CListBoxListener{
public:
	virtual void listBoxChanged(ListBox* lb, int row) = 0;
};