#include "standard.h"

flare::asset::Model obj::read(const char *name) {

	flare::asset::Model model;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool succes = tinyobj::LoadObj(shapes, materials, err, name);

	if (!succes) {
		print::e(err.c_str());
		exit(1);
	}

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

	return model;
}

void flare::asset::Model::_load() {

	// NOTE: This is just to make sure everything compiles
}
