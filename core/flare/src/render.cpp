#include "flare/flare.h"

flare::render::State state;

void flare::render::init() {

    // Enable MSAA
    glEnable(GL_MULTISAMPLE);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // glm::vec3(1.0f, 0.9f, 0.7f);
    // state.light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    state.light.direction = glm::vec3(-50.0f, -50.0f, -50.0f);

    state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f);
    state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
}

void flare::render::update() {

    state.view = glm::lookAt(state.camera.position, state.camera.position + state.camera.front, state.camera.up);

    state.projection = glm::perspectiveFov(glm::radians(90.0f), (float)getSettings()->resolution.x, (float)getSettings()->resolution.y , 0.1f, 100.0f);

    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void flare::render::setShader(flare::asset::Shader *shader) {

    if (state.shader != shader->id) {

	glUseProgram(shader->id);
	glUniform1i(glGetUniformLocation(shader->id, "material.diffuse"),  0);
	glUniform1i(glGetUniformLocation(shader->id, "material.normal"), 1);
	glUniform1i(glGetUniformLocation(shader->id, "material.specular"), 2);
	// glUniform1i(glGetUniformLocation(shader->id, "material.emission"), 3);
	state.shader = shader->id;
	
    }
}

// void flare::render::setModel(flare::asset::Model *model) {
//
//     glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
//     state.vbo = model->vbo;
//
//     glBindVertexArray(model->vao);
//     state.vao = model->vao;
// }

// void flare::render::setMaterial(flare::asset::Material *material) {
//
// 	glActiveTexture(GL_TEXTURE0);
// 	glBindTexture(GL_TEXTURE_2D, material->diffuse->id);
//
// 	glActiveTexture(GL_TEXTURE1);
// 	glBindTexture(GL_TEXTURE_2D, material->normal->id);
//
// 	glActiveTexture(GL_TEXTURE2);
// 	glBindTexture(GL_TEXTURE_2D, material->specular->id);
//
// 	// glActiveTexture(GL_TEXTURE3);
// 	// glBindTexture(GL_TEXTURE_2D, material->emission->id);
// }

flare::render::State *flare::render::getState() {
    return &state;
}

flare::render::Camera *flare::render::getCamera() {
    return &state.camera;
}

float flare::render::getDeltaTime() {
    return state.deltaTime;
}

