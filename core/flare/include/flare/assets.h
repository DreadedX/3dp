#ifndef ASSETS_H
#define ASSETS_H

namespace flare {

	/** @brief Asset handeling */
	namespace asset {

		/** @brief Base used for all assets */
		struct Asset {

			/** @brief Name of asset file */
			std::string name = "";

			/** @brief All assets need a load function */
			virtual void _load()=0;

			/** Asset destructor */
			virtual ~Asset() { }
		};

		/** @brief List of all loaded assets
			@todo This needs to be private, but needs to be accesible by load<T>() */
		extern std::vector<Asset*> assetList;
		/** @brief Hash map of all loaded asset, to make sure we are not loading assets that are already loaded
			@todo This needs to be private, but needs to be accesible by load<T>() */
		extern std::map<std::string, Asset*> assetMap;

		/** @brief Load an asset
			@tparam T - Asset type to load
			@param name - Name of the asset to load */
		template<typename T>
			T *load(std::string name) {

				if (assetMap.find(name) != assetMap.end()) {

					// print::d("'%s' already loaded", name.c_str());
					return reinterpret_cast<T*>(assetMap[name]);
				}

				print::d("Loading asset '%s'", name.c_str());

				T *asset = new T();
				asset->name = name;

				asset->_load();

				assetList.push_back(asset);
				assetMap[name] = asset;
				return asset;
			}

		/** @brief Reload all assets */
		void reload();
		/** @brief Close all loaded assets
			@todo Make sure that all assets have proper clean-up */
		void close();
	}
}

#include "assets/shader.h"
#include "assets/texture.h"
#include "assets/model.h"

#endif

