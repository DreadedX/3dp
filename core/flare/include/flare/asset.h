#ifndef ASSET_H
#define ASSET_H

namespace flare {

    namespace asset {

	struct Asset {

	    std::string name = "";

	    virtual void _load()=0;
	};

	extern std::vector<Asset*> assetList;

	struct ShaderAsset : Asset {

	    GLuint id;

	    void _load() override;
	};

	struct TextureAsset : Asset {

	    GLuint id;

	    void _load() override;
	};
	
	template<typename T>
	T *load(std::string name) {

	    T *asset = new T();
	    asset->name = name;

	    asset->_load();

	    assetList.push_back(asset);
	    return asset;
	}

	void reload();
    }
}

#endif

