#include "EffectList.h"

#include<string>
#include<fstream>
#include<streambuf>
#include<algorithm>
#include"HexFunc.h"

using std::max;

#define NULL_CHECK(value, defaultValue) value.is_null() ? defaultValue : value


EffectList::EffectList(){
	reload();
}

void EffectList::reload(){
	readJsonObj();
	createEffects();
}

void EffectList::readJsonObj(){

	std::ifstream t("C:\\Users\\Daniel\\Music\\RolandControl\\effectlist.json");
	std::string effectListFile((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	jsonObj = json::parse(effectListFile);
}

void EffectList::createEffects(){

	effects.clear();


	// Get maximum category index
	int maxIndex = 0;
	for(auto& j : jsonObj.items())
		maxIndex = max(j.value().at(0)["index"].get<int>(), maxIndex);


	// Counter for ordering categories
	for(int i = 0; i <= maxIndex; i++){

		// Iterate categories
		for(auto& j : jsonObj.items()){

			// Check that category order matches
			if(i != j.value().at(0)["index"].get<int>())
				continue;


			// Create effect list for category
			vector<Effect> efx;

			// Iterate effects
			for(int k = 1; k < j.value().size(); k++){
				Effect effect;
				auto t = j.value().at(k);


				// Name
				string s = t["name"].get<string>();
				effect.name = s;

				// EFX type bytes
				string s1 = t["msb"].get<string>();
				string s2 = t["lsb"].get<string>();
				effect.msb = hexStringToByte(s1);
				effect.lsb = hexStringToByte(s2);

				// Part parameter indexes
				effect.pparam1 = NULL_CHECK(t["pparam1"], -1).get<int>();
				effect.pparam2 = NULL_CHECK(t["pparam2"], -1).get<int>();

				// Value is one greater in file
				if(effect.pparam1 != -1)	effect.pparam1--;
				if(effect.pparam2 != -1)	effect.pparam2--;


				// Copy parameters from a previous effect in the same category
				auto copyParams = t["parameters_copy"];

				if(!copyParams.is_null()){
					
					// Copy from effect with given name
					for(auto& e : efx){
						if(e.name == copyParams.get<string>()){
							effect.params = e.params;
							break;
						}
					}

					efx.push_back(effect);
					continue;
				}

				// Create parameter list
				vector<EffectParameter> params;

				// Iterate parameters
				auto pms = t["parameters"];

				for(auto l = pms.begin(); l != pms.end(); l++){
					auto p = *l;

					EffectParameter param;

					// Name
					string s3 = p["name"].get<string>();
					param.name = s3;

					// Type
					string s4 = p["type"].get<string>();
					if(s4 == "SLIDER")			param.type = EffectParameter::SLIDER;
					else if(s4 == "SLIDER_H")	param.type = EffectParameter::SLIDER_H;
					else if(s4 == "SLIDER_V")	param.type = EffectParameter::SLIDER_V;
					else if(s4 == "BUTTON")		param.type = EffectParameter::BUTTON;
					else if(s4 == "LISTBOX")	param.type = EffectParameter::LISTBOX;

					// Position
					param.x = p["x"].get<int>();
					param.y = p["y"].get<int>();

					// Default value
					param.defaultValue = NULL_CHECK(p["default"], 0).get<int>();

					// Range (for sliders)
					param.min		= NULL_CHECK(p["min"], 0).get<int>();
					param.max		= NULL_CHECK(p["max"], -1).get<int>();

					// Default range
					// Display range and suffix
					param.dispMin	= NULL_CHECK(p["dispMin"], -1).get<double>();
					param.dispMax	= NULL_CHECK(p["dispMax"], -1).get<double>();
					string s5		= NULL_CHECK(p["suffix"], "");
					param.suffix = s5;

					// Size (for listbox)
					param.width		= NULL_CHECK(p["width"], 80).get<int>();
					param.height	= NULL_CHECK(p["height"], 80).get<int>();

					// Value list (for sliders/listboxes)
					vector<string> values;
					auto vals = p["values"];

					for(auto v = vals.begin(); v != vals.end(); v++)
						values.push_back((*v).get<string>());

					param.values = values;


					if(param.max == -1){
						// Set max value to number of values if not set
						param.max = !values.empty() ? values.size() - 1 : 127;

						// Max is always 1 for buttons
						if(param.type == EffectParameter::BUTTON)
							param.max = 1;
					}

					// Add parameter
					params.push_back(param);
				}

				// Set parameters
				effect.params = params;
				efx.push_back(effect);
			}

			// Create category
			EffectCategory c;
			c.name = j.key();
			c.effects = efx;

			effects.push_back(c);
		}
	}
}

vector<EffectCategory>& EffectList::getEffects(){
	return effects;
}

vector<string> EffectList::getCategoryNames(){

	vector<string> names;

	for(auto i = effects.begin(); i != effects.end(); i++)
		names.push_back((*i).name);

	return names;
}

vector<string> EffectList::getEffectNames(int cat){

	vector<string> names;

	vector<Effect> efx = effects[cat].effects;

	for(auto i = efx.begin(); i != efx.end(); i++)
		names.push_back((*i).name);

	return names;
}