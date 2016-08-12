#include <random>

#include <GL/gl.h>

#include "tiny_obj_loader.h"
#include "glm/glm.hpp"

#include "flare/assets/model.h"

#include "plugin.h"

namespace obj {

		struct Model;

		/** @brief Structs needed for models */
		namespace model {

			/** @brief Mesh data
				@todo Make this use a material instead of storing the texture on its own */
			struct Mesh {

				glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
				glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
				GLfloat shininess = 0.0f;

				std::string diffuseMap = "";
				std::string specularMap = "";

				/** @brief List of all the vertices in this mesh */
				Array<flare::asset::model::Vertex> vertices;
				/** @brief List of all the indices in this mesh */
				Array<GLuint> indices;
			};
		}

		/** @brief Asset struct containing 3d model info */
		struct Model : flare::asset::Asset {

			/** @brief List of all meshes in this model */
			Array<model::Mesh*> meshes;

			/** @brief Load model data */
			void _load() override {}

			/** @brief Model destructor
				Makes sure all meshes get deleted properly */
			~Model() {

				/** @todo Should probably also delete opengl buffers (this currently only gets called on shutdown, so causes no issues */
				for (model::Mesh *mesh : meshes) {

					delete mesh;
				}
			}
		};

	Model read(const char *name);
}

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
		print_e("%s", err);
		exit(1);
	}

	for (uint i = 0; i < shapes.size(); i++) {

		model::Mesh *mesh = new model::Mesh;
		model.meshes.add(mesh);

		// Increase the increment size, because we know we are going to be dealing with a lot of data
		mesh->vertices.resize(shapes[i].mesh.positions.size() / 3);
		mesh->indices.resize(shapes[i].mesh.indices.size());

		assert((shapes[i].mesh.indices.size() % 3) == 0);
		for (uint v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {

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

			mesh->vertices.add(vertex);

		}

		for (uint f = 0; f < shapes[i].mesh.indices.size(); f++) {

			mesh->indices.add(shapes[i].mesh.indices[f]);
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

			print_w("Model '%s' is missing a material", name);
		}
	}

	return model;
}

void load(std::string assetName, std::string filePath, Array<flux::File*> *files) {

	flux::File *file = new flux::File;
	files->add(file);
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

