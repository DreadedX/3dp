#ifndef ASSETS_H
#define ASSETS_H

namespace flare {

    namespace asset {

	struct Asset {

	    std::string name = "";

	    virtual void _load()=0;

	    virtual ~Asset() { }
	};

	extern std::vector<Asset*> assetList;
	extern std::map<std::string, Asset*> assetMap;

	template<typename T>
	T *load(std::string name) {

	    if (assetMap.find(name) != assetMap.end()) {

		print::d("'%s' already loaded", name.c_str());
		return reinterpret_cast<T*>(assetMap[name]);
	    }

	    print::d("Loading '%s'", name.c_str());

	    T *asset = new T();
	    asset->name = name;

	    asset->_load();

	    assetList.push_back(asset);
	    assetMap[name] = asset;
	    return asset;
	}

	void reload();
	void close();
    }
}

#include "assets/shader.h"
#include "assets/texture.h"
#include "assets/material.h"
#include "assets/model.h"

#endif

