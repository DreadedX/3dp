#include "flare/flare.h"

std::vector<flare::asset::Asset*> flare::asset::assetList;
std::map<std::string, flare::asset::Asset*> flare::asset::assetMap;

void flare::asset::reload() {

	flux::reload();

	for (Asset *asset : assetList) {

		print::d("Reloading asset: %s", asset->name.c_str());
		asset->_load();
	}
}

void flare::asset::close() {

	for (Asset *asset : assetList) {

		delete asset;
		asset = nullptr;
	}

	flux::close();
}
