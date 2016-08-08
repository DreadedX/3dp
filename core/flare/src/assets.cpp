#include "extra/array.h"
#include "flux/read.h"
#include "flare/assets.h"

Array<flare::asset::Asset*> flare::asset::assetList;
std::unordered_map<std::string, flare::asset::Asset*> flare::asset::assetMap;
Allocator *flare::asset::asset_allocator;

void flare::asset::init(Allocator *_allocator) {

	asset_allocator = _allocator;
}

void flare::asset::reload() {

	flux::read::reload();

	for (Asset *asset : assetList) {

		print_d("Reloading asset: %s", asset->name.c_str());
		asset->_load();
	}
}

void flare::asset::close() {

	for (Asset *asset : assetList) {

		// delete asset;
		allocator::make_delete<Asset>(*asset_allocator, *asset);
		asset = nullptr;
	}
}
