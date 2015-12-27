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

	    glm::vec3 normal;
	    normal.x = meshAi->mNormals[i].x;
	    normal.y = meshAi->mNormals[i].y;
	    normal.z = meshAi->mNormals[i].z;
	    vertex.normal = normal;

	    glm::vec3 tangent;
	    tangent.x = meshAi->mTangents[i].x;
	    tangent.y = meshAi->mTangents[i].y;
	    tangent.z = meshAi->mTangents[i].z;
	    vertex.tangent = tangent;

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

	glGenVertexArrays(1, &mesh->vao);
	glGenBuffers(1, &mesh->vbo);
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
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(flare::asset::model::Vertex), (GLvoid*)offsetof(flare::asset::model::Vertex, tangent));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(flare::asset::model::Vertex), (GLvoid*)offsetof(flare::asset::model::Vertex, texCoords));

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
    std::string fileName = "./" + name + ".obj";
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
	print::e("AssImp error: %s", import.GetErrorString());
	exit(-1);
    }

    for (model::Mesh *mesh : meshes) {
	if (mesh->vao != 0) {
	    glDeleteVertexArrays(1, &mesh->vao);
	}
	if (mesh->vbo != 0) {
	    glDeleteBuffers(1, &mesh->vbo);
	}
	if (mesh->ebo != 0) {
	    glDeleteBuffers(1, &mesh->ebo);
	}
	delete mesh;
    }
    meshes.clear();

    processNode(scene->mRootNode, scene, this);
}
