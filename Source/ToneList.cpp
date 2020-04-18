#include "ToneList.h"

#include<string>
#include<fstream>
#include<streambuf>


ToneList::ToneList(){
	reload();
}

void ToneList::reload(){
	readJsonObj();
	createTones();
}

void ToneList::readJsonObj(){

	std::ifstream t("C:\\Users\\Daniel\\Music\\RolandControl\\tonelist.json");
	std::string tonelistFile((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	jsonObj = json::parse(tonelistFile);
}

void ToneList::createTones(){

	tones.clear();

	// Counter for ordering categories
	for(int i = 0; i < jsonObj.size(); i++){

		// Iterate categories
		for(auto& j : jsonObj.items()){

			// Check that category order matches
			if(i != j.value().at(0)["index"])
				continue;


			// Create tone list for category
			vector<Tone> tns;

			// Iterate tones
			for(int k = 1; k < j.value().size(); k++){
				Tone tone;
				auto t = j.value().at(k);

				string s = t["name"];
				tone.name = s;

				int n = t["pc"];
				// PC is one greater in tonelist file
				tone.pc = n - 1;

				n = t["msb"];
				tone.msb = n;

				n = t["lsb"];
				tone.lsb = n;

				tns.push_back(tone);
			}

			// Create category
			ToneCategory c;
			c.name = j.key();
			c.tones = tns;

			tones.push_back(c);
		}
	}
}

vector<ToneCategory>& ToneList::getTones(){
	return tones;
}

vector<string> ToneList::getCategoryNames(){

	vector<string> names;

	for(auto i = tones.begin(); i != tones.end(); i++)
		names.push_back((*i).name);

	return names;
}

vector<string> ToneList::getToneNames(int cat){

	vector<string> names;

	vector<Tone> tns = tones[cat].tones;

	for(auto i = tns.begin(); i != tns.end(); i++)
		names.push_back((*i).name);

	return names;
}