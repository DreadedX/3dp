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

obj::Model obj::read(const char *name) {

	Model model;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string basePath;
	const size_t last_slash_idx = std::string(name).rfind('/');
	if (std::string::npos != last_slash_idx)
	{
		basePath = std::string(name).substr(0, last_slash_idx);
	}
	basePath += "/";

	std::string err;
	/** @todo Make this not hardcoded */
	bool succes = tinyobj::LoadObj(shapes, materials, err, name, basePath.c_str());
	if (!succes) {
		print::e(err.c_str());
		exit(1);
	}

	for (size_t i = 0; i < shapes.size(); i++) {

		model::Mesh *mesh = new model::Mesh;
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

		// I assume each mesh can only have one material
		int materialId = shapes[i].mesh.material_ids[0];

		if (materialId >= 0) {

			tinyobj::material_t material = materials[materialId];

			mesh->diffuseColor = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
			mesh->specularColor = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
			mesh->shininess = material.shininess;

			mesh->diffuseMap = material.diffuse_texname;
			mesh->specularMap = material.specular_texname;
		} else {

			print::w("Model '%s' is missing a material", name);
		}
	}

	return model;
}

void load(std::string assetName, std::string filePath, std::vector<flux::FileWrite*> *files) {

	flux::FileWrite *file = new flux::FileWrite;
	files->push_back(file);
	file->name = assetName;

	obj::Model model = obj::read(filePath.c_str());

	/** @todo Should this really be stored in extra */
	ulong meshCount = model.meshes.size();
	file->dataSize = sizeof(ulong) + meshCount * (sizeof(ulong) + sizeof(ulong));

	for (ulong j = 0; j < meshCount; ++j) {

		ulong vertexCount = model.meshes[j]->vertices.size();
		ulong indexCount = model.meshes[j]->indices.size();

		file->dataSize += vertexCount * sizeof(flare::asset::model::Vertex);
		file->dataSize += indexCount * sizeof(GLuint);
		file->dataSize += sizeof(glm::vec3);
		file->dataSize += sizeof(glm::vec3);
		file->dataSize += sizeof(GLfloat);
		file->dataSize += sizeof(byte);
		file->dataSize += model.meshes[j]->diffuseMap.length();
		file->dataSize += sizeof(byte);
		file->dataSize += model.meshes[j]->specularMap.length();
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

	for (ulong j = 0; j < meshCount; ++j) {

		ulong vertexCount = model.meshes[j]->vertices.size();
		ulong indexCount = model.meshes[j]->indices.size();

		for (uint i = 0; i < vertexCount; ++i) {

			/** @todo Why am i copying this? */
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

		byte *tempData = new byte[sizeof(glm::vec3)];
		memcpy(tempData, &model.meshes[j]->diffuseColor, sizeof(glm::vec3));
		for (uint n = 0; n < sizeof(glm::vec3); ++n) {

			file->data[n + offset] = tempData[n];
		}
		offset += sizeof(glm::vec3);
		delete[] tempData;

		tempData = new byte[sizeof(glm::vec3)];
		memcpy(tempData, &model.meshes[j]->specularColor, sizeof(glm::vec3));
		for (uint n = 0; n < sizeof(glm::vec3); ++n) {

			file->data[n + offset] = tempData[n];
		}
		offset += sizeof(glm::vec3);
		delete[] tempData;

		tempData = new byte[sizeof(GLfloat)];
		memcpy(tempData, &model.meshes[j]->shininess, sizeof(GLfloat));
		for (uint n = 0; n < sizeof(GLfloat); ++n) {

			file->data[n + offset] = tempData[n];
		}
		offset += sizeof(GLfloat);
		delete[] tempData;

		file->data[offset] = static_cast<byte>(model.meshes[j]->diffuseMap.length());
		offset += sizeof(byte);

		for (uint n = 0; n < model.meshes[j]->diffuseMap.length(); ++n) {

			file->data[n + offset] = model.meshes[j]->diffuseMap[n];
		}
		offset += model.meshes[j]->diffuseMap.length();

		file->data[offset] = static_cast<byte>(model.meshes[j]->specularMap.length());
		offset += sizeof(byte);

		for (uint n = 0; n < model.meshes[j]->specularMap.length(); ++n) {

			file->data[n + offset] = model.meshes[j]->specularMap[n];
		}
		offset += model.meshes[j]->specularMap.length();
	}

	// print::d("OBJ hex:");
    //
	// for (uint i = 0; i < file->dataSize; ++i) {
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

