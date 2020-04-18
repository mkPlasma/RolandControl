#pragma once

#include<string>
#include<vector>
#include<nlohmann/json.hpp>

using std::string;
using std::vector;
using nlohmann::json;


struct EffectParameter{
	enum ParameterType{SLIDER, SLIDER_H, SLIDER_V, BUTTON, LISTBOX};

	string name;
	ParameterType type;

	int x, y;

	int defaultValue;

	// For slider
	string suffix;
	int min, max;
	double dispMin, dispMax;

	// For list box
	int width, height;

	// For slider/list box
	vector<string> values;
};

struct Effect{
	string name;

	// Type bytes
	int msb;
	int lsb;

	// Part parameter indexes
	int pparam1;
	int pparam2;

	// Parameter list
	vector<EffectParameter> params;
};

struct EffectCategory{
	string name;
	vector<Effect> effects;
};


class EffectList{

	json jsonObj;
	vector<EffectCategory> effects;

	void readJsonObj();
	void createEffects();

public:
	EffectList();

	void reload();

	vector<EffectCategory>& EffectList::getEffects();

	vector<string> EffectList::getCategoryNames();
	vector<string> EffectList::getEffectNames(int cat);
};