#ifndef ASSET_H
#define ASSET_H

namespace flare {

    namespace asset {

	struct Asset {

	    std::string name = "";

	    virtual void _load()=0;

	    virtual ~Asset() { }
	};

	extern std::vector<Asset*> assetList;

	// TODO: Find a better solution to storing all attrib and uniform locators
	struct ShaderAsset : Asset {

	    GLuint id;

	    struct {

		GLuint position = 0;
		GLuint texture = 0;
		GLuint normal = 0;

		GLuint lightColor = 0;
		GLuint diffuseLocation = 0;
		GLuint specularLocation = 0;

		GLuint model = 0;
		GLuint view = 0;
		GLuint projection = 0;

	    } locations;

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
	void close();
    }
}

#endif

