#include "flare/flare.h"

void flare::asset::Material::_load() {

    jsoncons::json materialJson;
    if (name != "") {

	flux::FileLoad *materialConfigFile = flux::get(name);
	const char *materialConfig = reinterpret_cast<const char*>(materialConfigFile->get(true));

	materialJson = jsoncons::json::parse_string(materialConfig);

	delete[] materialConfig;
    }  else {

	materialJson = jsoncons::json::parse_string("");
    }
    
    diffuse = asset::load<Texture>(materialJson.get("diffuse", "").as<std::string>());
    specular = asset::load<Texture>(materialJson.get("specular", "").as<std::string>());
    normal = asset::load<Texture>(materialJson.get("normal", "").as<std::string>());
    //emission = asset::load<Texture>(materialJson.get("emission", "").as<std::string>());

    shininess = materialJson.get("shininess").as<float>();
}

