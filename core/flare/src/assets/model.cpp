#include <GL/glew.h>

#include "flux/file.h"
#include "flux/read.h"
#include "flare/engine.h"
#include "flare/assets/model.h"

/** @todo The majority of this code should move into fluxuate, in order to store the raw data for faster parsing at runtime
	@todo This uses to much data */

void clearMeshes(flare::asset::Model *model) {
	
	for (flare::asset::model::Mesh *mesh : model->meshes) {
		if (mesh->vao != 0) {
			glDeleteVertexArrays(1, &mesh->vao);
		}
		if (mesh->vbo != 0) {
			glDeleteBuffers(1, &mesh->vbo);
		}
		if (mesh->ebo != 0) {
			glDeleteBuffers(1, &mesh->ebo);
		}
		// delete mesh;
		allocator::make_delete<flare::asset::model::Mesh>(*flare::getState()->proxyAllocators.model, *mesh);
	}
	model->meshes.clear();

}

void flare::asset::Model::_load() {

	double timer = glfwGetTime();

	flux::File *modelFile = flux::read::get(name);
	// flux::FileLoad *materialFile = flux::get("base/" + name + "_material");

	byte *modelData = modelFile->load();
	// byte *materialData = materialFile->get();

	clearMeshes(this);

	uint offset = 0;

	ulong meshCount = 0;
	for (ulong i = 0; i < sizeof(ulong); ++i) {
		meshCount += modelData[i] << (i*8);
	}
	offset += sizeof(ulong);

	// ulong *vertexCount = new ulong[meshCount];
	// ulong *indexCount = new ulong[meshCount];
	ulong *vertexCount = allocator::make_new_array<ulong>(*getState()->proxyAllocators.model, meshCount);
	ulong *indexCount = allocator::make_new_array<ulong>(*getState()->proxyAllocators.model, meshCount);

	for (ulong i = 0; i < meshCount; ++i) {
		vertexCount[i] = 0;
		indexCount[i] = 0;
	}

	for (ulong i = 0; i < meshCount; ++i) {
		
		for (ulong j = 0; j < sizeof(ulong); ++j) {
			vertexCount[i] = vertexCount[i] + (modelData[j + offset] << (j*8));
		}
		offset += sizeof(ulong);

		for (ulong j = 0; j < sizeof(ulong); ++j) {
			indexCount[i] = indexCount[i] + (modelData[j + offset] << (j*8));
		}
		offset += sizeof(ulong);
	}

	for (ulong i = 0; i < meshCount; ++i) {

		// model::Mesh *mesh = new model::Mesh;
		model::Mesh *mesh = allocator::make_new<model::Mesh>(*getState()->proxyAllocators.model);
		meshes.add(mesh);
		// mesh->vertices.resize(vertexCount[i]);
		mesh->vertices = allocator::make_new_array<model::Vertex>(*getState()->proxyAllocators.model, vertexCount[i]);
		// mesh->indices.resize(indexCount[i]);
		mesh->indices = allocator::make_new_array<GLuint>(*getState()->proxyAllocators.model, indexCount[i]);

		for (ulong j = 0; j < vertexCount[i]; ++j) {

			model::Vertex *vertex = &mesh->vertices[j];

			// byte *tempData = new byte[sizeof(glm::vec3)];
			byte *tempData = allocator::make_new_array<byte>(*getState()->proxyAllocators.model, sizeof(glm::vec3));
			for (uint n = 0; n < sizeof(glm::vec3); ++n) {
				tempData[n] = modelData[n + offset];
			}
			offset += sizeof(glm::vec3);
			memcpy(&vertex->position, tempData, sizeof(glm::vec3));
			// delete[] tempData;
			
			// tempData = new byte[sizeof(glm::vec3)];
			for (uint n = 0; n < sizeof(glm::vec3); ++n) {
				tempData[n] = modelData[n + offset];
			}
			offset += sizeof(glm::vec3);
			memcpy(&vertex->normal, tempData, sizeof(glm::vec3));
			// delete[] tempData;
			
			// tempData = new byte[sizeof(glm::vec2)];
			for (uint n = 0; n < sizeof(glm::vec2); ++n) {
				tempData[n] = modelData[n + offset];
			}
			offset += sizeof(glm::vec2);
			memcpy(&vertex->texCoords, tempData, sizeof(glm::vec2));

			// delete[] tempData;
			allocator::make_delete_array<byte>(*getState()->proxyAllocators.model, tempData);
		}
		for (ulong j = 0; j < indexCount[i]; ++j) {

			GLuint index = 0;

			for (uint n = 0; n < sizeof(GLuint); ++n) {

				index += modelData[n + offset] << (n*8);
			}
			offset += sizeof(GLuint);

			memcpy(&mesh->indices[j], &index, sizeof(GLuint));
		}

		// byte *tempData = new byte[sizeof(glm::vec3)];
		byte *tempData = allocator::make_new_array<byte>(*getState()->proxyAllocators.model, sizeof(glm::vec3));
		for (uint n = 0; n < sizeof(glm::vec3); ++n) {
			tempData[n] = modelData[n + offset];
		}
		offset += sizeof(glm::vec3);
		memcpy(&mesh->diffuseColor, tempData, sizeof(glm::vec3));
		// delete[] tempData;

		// tempData = new byte[sizeof(glm::vec3)];
		for (uint n = 0; n < sizeof(glm::vec3); ++n) {
			tempData[n] = modelData[n + offset];
		}
		offset += sizeof(glm::vec3);
		memcpy(&mesh->specularColor, tempData, sizeof(glm::vec3));
		// delete[] tempData;

		// tempData = new byte[sizeof(GLfloat)];
		for (uint n = 0; n < sizeof(GLfloat); ++n) {
			tempData[n] = modelData[n + offset];
		}
		offset += sizeof(GLfloat);
		memcpy(&mesh->shininess, tempData, sizeof(GLfloat));

		mesh->shininess = mesh->shininess == 0 ? 1.0f : mesh->shininess;

		// delete[] tempData;
		allocator::make_delete_array<byte>(*getState()->proxyAllocators.model, tempData);

		byte length = modelData[offset];
		offset += sizeof(byte);

		// tempData = new byte[length + 2];
		tempData = allocator::make_new_array<byte>(*getState()->proxyAllocators.model, length + 2);
		for (uint n = 0; n < length; ++n) {
			tempData[n] = modelData[n + offset];
		}
		tempData[length] = 0x00;
		tempData[length + 1] = 0x00;
		// if (length != 0) {

			mesh->diffuse = load<Texture>(reinterpret_cast<const char*>(tempData));
		// }
		// delete[] tempData;
		offset += length;

		length = modelData[offset];
		offset += sizeof(byte);

		// tempData = new byte[length + 2];
		for (uint n = 0; n < length; ++n) {
			tempData[n] = modelData[n + offset];
		}
		tempData[length] = 0x00;
		tempData[length + 1] = 0x00;
		// if (length != 0) {
		
			mesh->specular = load<Texture>(reinterpret_cast<const char*>(tempData));
		// }
		// delete[] tempData;
		allocator::make_delete_array<byte>(*getState()->proxyAllocators.model, tempData);
		offset += length;

		glGenVertexArrays(1, &mesh->vao);
		glGenBuffers(1, &mesh->vbo);
		glGenBuffers(1, &mesh->ebo);

		glBindVertexArray(mesh->vao);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexCount[i] * sizeof(flare::asset::model::Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount[i] * sizeof(GLuint), &mesh->indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(flare::asset::model::Vertex), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(flare::asset::model::Vertex), (GLvoid*)offsetof(flare::asset::model::Vertex, normal));

		/** @todo Figure out a way to generate these, if they are even needed */
		// glEnableVertexAttribArray(2);
		// glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(flare::asset::model::Vertex), (GLvoid*)offsetof(flare::asset::model::Vertex, tangent));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(flare::asset::model::Vertex), (GLvoid*)offsetof(flare::asset::model::Vertex, texCoords));

		// printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
		// printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
		// printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
		// for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
		// 	printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3*f+0], shapes[i].mesh.indices[3*f+1], shapes[i].mesh.indices[3*f+2], shapes[i].mesh.material_ids[f]);
		// }
        //
		// printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
		// assert((shapes[i].mesh.positions.size() % 3) == 0);
		// for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
		// 	printf("  v[%ld] = (%f, %f, %f)\n", v,
		// 			shapes[i].mesh.positions[3*v+0],
		// 			shapes[i].mesh.positions[3*v+1],
		// 			shapes[i].mesh.positions[3*v+2]);
		// }
		

		mesh->indexSize = indexCount[i];
		// mesh->indices.clear();
		allocator::make_delete_array<GLuint>(*getState()->proxyAllocators.model, mesh->indices);
		// mesh->vertices.clear();
		allocator::make_delete_array<model::Vertex>(*getState()->proxyAllocators.model, mesh->vertices);
	}

	// delete[] vertexCount;
	// delete[] indexCount;
	
	allocator::make_delete_array(*getState()->proxyAllocators.model, indexCount);
	allocator::make_delete_array(*getState()->proxyAllocators.model, vertexCount);

	print_d("File memory usage: %i bytes (%i%%)", getState()->proxyAllocators.flux->getUsedMemory(),
			(int)(getState()->proxyAllocators.flux->getUsedMemory()*100/getState()->proxyAllocators.flux->getSize()));

	// delete[] modelData;
	allocator::make_delete_array<byte>(*getState()->proxyAllocators.flux, modelData);

	double delta = glfwGetTime() - timer;
	print_d("Model loaded in %f seconds", delta);
}

flare::asset::Model::~Model() {

	clearMeshes(this);
}
