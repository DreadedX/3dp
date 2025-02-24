#ifndef ASSETS_H
#define ASSETS_H

#include <unordered_map>

#include "extra/array.h"
#include "extra/allocator.h"

/** @brief Asset handeling */
namespace flare::asset {

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
	extern Array<Asset*> assetList;
	/** @brief Hash map of all loaded asset, to make sure we are not loading assets that are already loaded
	  @todo This needs to be private, but needs to be accesible by load<T>() */
	extern std::unordered_map<std::string, Asset*> assetMap;

	extern Allocator *asset_allocator;

	void init(Allocator *_allocator);

	/** @brief Load an asset
	  @tparam T - Asset type to load
	  @param name - Name of the asset to load */
	template<typename T>
		T *load(std::string name) {

			if (assetMap.find(name) != assetMap.end()) {

				// print::d("'%s' already loaded", name.c_str());
				return reinterpret_cast<T*>(assetMap[name]);
			}

			print_d("Loading asset '%s'", name.c_str());

			// T *asset = new T();
			T* asset = allocator::make_new<T>(*asset_allocator);

			asset->name = name;

			asset->_load();

			assetList.add(asset);
			assetMap[name] = asset;
			return asset;
		}

	/** @brief Reload all assets */
	void reload();
	/** @brief Close all loaded assets
	  @todo Make sure that all assets have proper clean-up */
	void close();
}

#endif

