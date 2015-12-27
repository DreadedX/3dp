#include "flare/flare.h"

GLfloat verticesOld[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

void flare::asset::Model::draw(Shader *shader, Material *material, component::Object *parent) {

    // render::setModel(model);
    render::setShader(shader);
    render::setMaterial(material);

    for (model::Mesh *mesh : meshes) {

	mesh->_draw(shader, material, parent);
    }
}

void flare::asset::model::Mesh::_draw(Shader *shader, Material *material, component::Object *parent) {
    
    glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBindBuffer(GL_ARRAY_BUFFER, ebo);

    // print::d("%f, %f, %f", vertices[0].position.x, vertices[0].position.y, vertices[0].position.z);
    // print::d("%s", shader->name.c_str());

    flare::render::State *state = flare::render::getState();

    {
	glUniformMatrix4fv(shader->locations.view, 1, GL_FALSE, glm::value_ptr(state->view));
	glUniformMatrix4fv(shader->locations.projection, 1, GL_FALSE, glm::value_ptr(state->projection));

	glUniform3fv(shader->locations.light.position, 1, glm::value_ptr(state->light.position));
	glUniform3fv(shader->locations.light.ambient, 1, glm::value_ptr(state->light.ambient));
	glUniform3fv(shader->locations.light.diffuse, 1, glm::value_ptr(state->light.diffuse));
	glUniform3fv(shader->locations.light.specular, 1, glm::value_ptr(state->light.specular));

	glUniform3fv(shader->locations.viewPosition, 1, glm::value_ptr(render::getCamera()->position));

	glUniform1f(shader->locations.material.shininess, material->shininess);
    }

    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, parent->position->position);
    if (parent->rotation != nullptr) {

	modelMatrix = glm::rotate(modelMatrix, parent->rotation->rotation, parent->rotation->rotationAxis);  
    }
    glUniformMatrix4fv(shader->locations.model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void processNode(aiNode *node, const aiScene *scene, flare::asset::Model *model) {

    print::d("Name: %s", node->mName.C_Str());

    for (GLuint i = 0; i < node->mNumMeshes; i++) {

	aiMesh *meshAi = scene->mMeshes[node->mMeshes[i]];
	flare::asset::model::Mesh *mesh = new flare::asset::model::Mesh;

	for (GLuint i = 0; i < meshAi->mNumVertices; i++) {

	    flare::asset::model::Vertex vertex;

	    glm::vec3 position;
	    position.x = meshAi->mVertices[i].x;
	    position.y = meshAi->mVertices[i].y;
	    position.z = meshAi->mVertices[i].z;
	    vertex.position = position;

	    print::d("Vertex (%i): %f, %f, %f", i, meshAi->mVertices[i].x, meshAi->mVertices[i].y, meshAi->mVertices[i].z);

	    glm::vec3 normal;
	    normal.x = meshAi->mNormals[i].x;
	    normal.y = meshAi->mNormals[i].y;
	    normal.z = meshAi->mNormals[i].z;
	    vertex.normal = normal;

	    if (meshAi->mTextureCoords[0]) {

		glm::vec2 vec;
		vec.x = meshAi->mTextureCoords[0][i].x;
		vec.y = meshAi->mTextureCoords[0][i].y;
		vertex.texCoords = vec;
	    } else {

		vertex.texCoords = glm::vec2(0.0f, 0.0f);
	    }

	    mesh->vertices.push_back(vertex);
	}

	for (GLuint i = 0; i < meshAi->mNumFaces; i++) {

	    aiFace face = meshAi->mFaces[i];
	    for (GLuint j = 0; j < face.mNumIndices; j++) {

		mesh->indices.push_back(face.mIndices[j]);
	    }
	}

	if (mesh->vao != 0) {

	    glDeleteVertexArrays(1, &mesh->vao);
	    mesh->vao = 0;
	}
	glGenVertexArrays(1, &mesh->vao);

	if (mesh->vbo != 0) {

	    glDeleteBuffers(1, &mesh->vbo);
	    mesh->vbo = 0;
	}
	glGenBuffers(1, &mesh->vbo);

	if (mesh->ebo != 0) {

	    glDeleteBuffers(1, &mesh->ebo);
	    mesh->ebo = 0;
	}
	glGenBuffers(1, &mesh->ebo);

	glBindVertexArray(mesh->vao);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(flare::asset::model::Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(GLuint), &mesh->indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(flare::asset::model::Vertex), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(flare::asset::model::Vertex), (GLvoid*)offsetof(flare::asset::model::Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(flare::asset::model::Vertex), (GLvoid*)offsetof(flare::asset::model::Vertex, texCoords));

	model->meshes.push_back(mesh);
    }
    for (GLuint i = 0; i < node->mNumChildren; i++) {

	processNode(node->mChildren[i], scene, model);
    }
}

void flare::asset::Model::_load() {

    // TODO: Are you allow to have modelless shaders
    // TODO: Check at which point this check should occure
    // if (name == "") {
	// return;
    // }

    // Generating and binding vao for the object
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile("./cube.obj", aiProcess_Triangulate || !scene->mRootNode);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
	print::e("AssImp error: %s", import.GetErrorString());
	exit(-1);
    }

    processNode(scene->mRootNode, scene, this);
}
