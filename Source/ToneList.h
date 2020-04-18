#pragma once

#include<string>
#include<vector>
#include<nlohmann/json.hpp>

using std::string;
using std::vector;
using nlohmann::json;


struct Tone{
	string name;
	int pc;
	int msb;
	int lsb;
};

struct ToneCategory{
	string name;
	vector<Tone> tones;
};


class ToneList{

	json jsonObj;
	vector<ToneCategory> tones;

	void readJsonObj();
	void createTones();

public:
	ToneList();

	void reload();

	vector<ToneCategory>& ToneList::getTones();

	vector<string> ToneList::getCategoryNames();
	vector<string> ToneList::getToneNames(int cat);
};