#include "flare/flare.h"

Array<flare::asset::Asset*> flare::asset::assetList;
std::map<std::string, flare::asset::Asset*> flare::asset::assetMap;
Allocator *flare::asset::asset_allocator;


void flare::asset::init(Allocator *_allocator) {

	asset_allocator = _allocator;
}

void flare::asset::reload() {

	flux::reload();

	for (Asset *asset : assetList) {

		print::d("Reloading asset: %s", asset->name.c_str());
		asset->_load();
	}
}

void flare::asset::close() {

	for (Asset *asset : assetList) {

		// delete asset;
		allocator::make_delete<Asset>(*asset_allocator, *asset);
		asset = nullptr;
	}

	flux::close();
}
