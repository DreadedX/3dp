/** @todo Figure out which of these are really needed */
// Standard library
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <assert.h>
#include <algorithm>
#include <ctime>

// System libraries
#include <png.h>
#include <zlib.h>

#include "jsoncons/json.hpp"

#include "tinyobjloader/tiny_obj_loader.h"

// Needed to be able to store assets in the same format as flare
#include "flare/flare.h"

// Debug
// #ifndef NDEBUG
//     #include "debug_new.h"
// #endif

#include "extra/extra.h"
#include "flux/flux.h"

#include "plugin_obj.h"
#include "plugin.h"

/** @note This only exists to make sure everything compiles */
void flare::asset::Model::_load() {}

flare::asset::Model obj::read(const char *name) {

	flare::asset::Model model;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string basePath;
	const size_t last_slash_idx = std::string(name).rfind('/');
	if (std::string::npos != last_slash_idx)
	{
		basePath = std::string(name).substr(0, last_slash_idx);
	}
	basePath += "/";

	print::d("%s", basePath.c_str());

	std::string err;
	/** @todo Make this not hardcoded */
	bool succes = tinyobj::LoadObj(shapes, materials, err, name, basePath.c_str());
	if (!succes) {
		print::e(err.c_str());
		exit(1);
	}

	print::d("Meshes: %i", shapes.size());

	for (size_t i = 0; i < shapes.size(); i++) {

		flare::asset::model::Mesh *mesh = new flare::asset::model::Mesh;
		model.meshes.push_back(mesh);

		assert((shapes[i].mesh.indices.size() % 3) == 0);
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {

			flare::asset::model::Vertex vertex;

			vertex.position.x = shapes[i].mesh.positions[3*v+0];
			vertex.position.y = shapes[i].mesh.positions[3*v+1];
			vertex.position.z = shapes[i].mesh.positions[3*v+2];

			// TODO: Calculate this when not included in model
			vertex.normal.x = shapes[i].mesh.normals[3*v+0];
			vertex.normal.y = shapes[i].mesh.normals[3*v+1];
			vertex.normal.z = shapes[i].mesh.normals[3*v+2];

			// TODO: DO not do this when the model is untextured
			// TODO: Figure out what happens and what to do with untextured models
			vertex.texCoords.x = shapes[i].mesh.texcoords[2*v+0];
			vertex.texCoords.y = shapes[i].mesh.texcoords[2*v+1];

			mesh->vertices.push_back(vertex);

		}

		for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) {

			mesh->indices.push_back(shapes[i].mesh.indices[f]);
		}
	}
	
	for (size_t i = 0; i < materials.size(); i++) {

		// diffuse color				Kd
		// diffuse specular color		Kd
		// shininess					Ns
		// diffuse map					map_Kd
		// specular map					map_Ks

		// printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
		// printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
		// printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
		// printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
		// printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
		// printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
		// printf("  material.Ns = %f\n", materials[i].shininess);
		// printf("  material.Ni = %f\n", materials[i].ior);
		// printf("  material.dissolve = %f\n", materials[i].dissolve);
		// printf("  material.illum = %d\n", materials[i].illum);
		// printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
		// printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
		// printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
		// printf("  material.map_Ns = %s\n", materials[i].specular_highlight_texname.c_str());
		// std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
		// std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
		// for (; it != itEnd; it++) {
		// 	printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
		// }
		// printf("\n");
	}

	return model;
}

void load(std::string filePath, flux::FileWrite *file) {

	print::d(file->name.c_str());

	flare::asset::Model model = obj::read(filePath.c_str());

	/** @todo Should this really be stored in extra */
	ulong meshCount = model.meshes.size();
	file->dataSize = sizeof(ulong) + meshCount * (sizeof(ulong) + sizeof(ulong));

	for (ulong j = 0; j < meshCount; ++j) {

		ulong vertexCount = model.meshes[j]->vertices.size();
		ulong indexCount = model.meshes[j]->indices.size();

		file->dataSize += vertexCount * sizeof(flare::asset::model::Vertex);
		file->dataSize += indexCount * sizeof(GLuint);
	}

	file->data = new byte[file->dataSize];
	uint offset = sizeof(ulong);

	for (uint i = 0; i < sizeof(ulong); ++i) {

		file->data[i] = meshCount >> (i*8);
	}

	for (ulong j = 0; j < meshCount; ++j) {

		ulong vertexCount = model.meshes[j]->vertices.size();
		ulong indexCount = model.meshes[j]->indices.size();

		for (uint i = 0; i < sizeof(ulong); ++i) {

			file->data[i + offset] = vertexCount >> (i*8);
		}
		for (uint i = 0; i < sizeof(ulong); ++i) {

			file->data[i + offset + sizeof(ulong)] = indexCount >> (i*8);
		}

		offset += 2*sizeof(ulong);
	}

	for (uint i = 0; i < file->dataSize - offset; ++i) {

		file->data[i + offset] = 0x0;
	}

	for (ulong j = 0; j < meshCount; ++j) {

		ulong vertexCount = model.meshes[j]->vertices.size();
		ulong indexCount = model.meshes[j]->indices.size();

		for (uint i = 0; i < vertexCount; ++i) {

			glm::vec3 position = model.meshes[j]->vertices[i].position;
			glm::vec3 normal = model.meshes[j]->vertices[i].normal;
			glm::vec2 texCoords = model.meshes[j]->vertices[i].texCoords;

			// TODO: This is system depended (byte order)
			// Write position
			byte *tempData = new byte[sizeof(glm::vec3)];
			memcpy(tempData, &position, sizeof(glm::vec3));

			for (uint n = 0; n < sizeof(glm::vec3); ++n) {

				file->data[n + offset] = tempData[n];
			}
			offset += sizeof(glm::vec3);
			delete[] tempData;

			// Write normal
			tempData = new byte[sizeof(glm::vec3)];
			memcpy(tempData, &normal, sizeof(glm::vec3));

			for (uint n = 0; n < sizeof(glm::vec3); ++n) {

				file->data[n + offset] = tempData[n];
			}
			offset += sizeof(glm::vec3);
			delete[] tempData;

			// Write texCoords
			tempData = new byte[sizeof(glm::vec2)];
			memcpy(tempData, &texCoords, sizeof(glm::vec2));

			for (uint n = 0; n < sizeof(glm::vec2); ++n) {

				file->data[n + offset] = tempData[n];
			}
			offset += sizeof(glm::vec2);
			delete[] tempData;
		}
		for (uint i = 0; i < indexCount; ++i) {

			GLuint index = model.meshes[j]->indices[i];

			// Write index
			for (uint n = 0; n < sizeof(GLuint); ++n) {

				file->data[n + offset] = index >> (n*8);
			}

			offset += sizeof(GLuint);
		}
	}

	// print::d("OBJ hex:");
    //
	// for (uint i = 0; i < 128; ++i) {
    //
	// 	if (i % 32 == 0 && i != 0) {
	// 		printf("\n");
	// 	}
    //
	// 	printf("0x%02X ", file->data[i]);
	// }
    //
	// printf("\n");

	assert(offset == file->dataSize);
}

Plugin plugin("OBJ Plugin", "Allows fluxuate to pack obj files in a format compatible with Flare", load);

extern "C" {

	Plugin getPlugin() {

		return plugin;
	}
}

