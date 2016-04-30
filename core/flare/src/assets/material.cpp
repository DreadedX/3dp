#include "flare/flare.h"

void flare::asset::Material::_load() {

	/** @todo This is kind of stupid, loading file data twice */
	flux::FileLoad *materialFile = flux::get(name);

	byte *materialData = materialFile->get();

	// DO STUFF
	
	delete[] materialData;

	diffuse = load<Texture>("base/rungholt-RGBA");
	specular = load<Texture>("base/rungholt-specular");
}

