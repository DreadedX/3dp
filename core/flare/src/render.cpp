#include <cassert>

#include <GL/glew.h>

#include "glm/gtc/matrix_transform.hpp"

#include "flare/state.h"
#include "flare/engine.h"
#include "flare/input.h"

GLfloat vertices[] = {
	// Left bottom triangle
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	// Right top triangle
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f
};

GLuint vao;

void flare::render::init() {

	getState()->render.sampleCount = 8;
	print_d("MSAA: %i", getState()->render.sampleCount);

	State::Render *render = &getState()->render;

	// Shaders need to have MSAA and resolution data
	getState()->mainState->renderPasses.add(new passes::Shadow);
	getState()->mainState->renderPasses.add(new passes::Basic);
	getState()->mainState->renderPasses.add(new passes::Skybox);
	getState()->mainState->renderPasses.add(new passes::ResolveMSAA);
	// There needs to be a pass that resolves the multisample texture to a normal texture
	// There needs to be a pass that generates geometry data that is not multisampled
	getState()->mainState->renderPasses.add(new passes::Geometry);
	getState()->mainState->renderPasses.add(new passes::SSAO);

	for(passes::Pass *pass : getState()->mainState->renderPasses) {

		pass->init();
	}

    // Enable MSAA
    // glEnable(GL_MULTISAMPLE);

    // Enable depth testing
    // glEnable(GL_DEPTH_TEST);

	// Enable transparency
    // glEnable(GL_BLEND);
	// glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // glm::vec3(1.0f, 0.9f, 0.7f);
    // state.light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    render->light.direction = glm::vec3(122.0f, -162.0f, -190.0f);

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    render->light.ambient = glm::vec3(0.02f, 0.02f, 0.02f) * color;
    render->light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * color;
    render->light.specular = glm::vec3(1.0f, 1.0f, 1.0f) * color;

    // state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f);
    // state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    // state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	
	/** @todo This needs to not use a model, but a hardcoded quad */
	render->quad = asset::load<asset::Model>("core/model/quad");

	GLuint vbo = 0;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
}

void debugRender(flare::GameState *) {

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLsizei width = (GLsizei)flare::getState()->settings.resolution.x;
		GLsizei height = (GLsizei)flare::getState()->settings.resolution.y;

		glDrawBuffer(GL_BACK);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		if (flare::input::keyCheck(GLFW_KEY_1)) {

			glBindFramebuffer(GL_READ_FRAMEBUFFER, flare::getState()->mainState->renderPasses[0]->fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, 1024*2, 1024*2, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		if (flare::input::keyCheck(GLFW_KEY_2)) {

			glBindFramebuffer(GL_READ_FRAMEBUFFER, flare::getState()->mainState->renderPasses[1]->fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		if (flare::input::keyCheck(GLFW_KEY_3)) {

			glBindFramebuffer(GL_READ_FRAMEBUFFER, flare::getState()->mainState->renderPasses[2]->fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		if (flare::input::keyCheck(GLFW_KEY_4)) {

			glBindFramebuffer(GL_READ_FRAMEBUFFER, flare::getState()->mainState->renderPasses[3]->fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		if (flare::input::keyCheck(GLFW_KEY_5)) {

			glBindFramebuffer(GL_READ_FRAMEBUFFER, flare::getState()->mainState->renderPasses[4]->fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		if (flare::input::keyCheck(GLFW_KEY_6)) {

			glBindFramebuffer(GL_READ_FRAMEBUFFER, flare::getState()->mainState->renderPasses[5]->fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
}

/** @todo Functions like these need unit testing */
float nfmod(float a, float b) {
	
	return a - b * floor(a / b);
}

void flare::render::draw(GameState *gameState) {

	State::Render *render = &getState()->render;

	// Calculate the camera angle
	// state.camera->rotation.y = nfmod(state.camera->rotation.y, 360);
	if (render->camera.rotation.y > 89.9) {
		render->camera.rotation.y = 89.9;
	}
	if (render->camera.rotation.y < -89.9) {
		render->camera.rotation.y = -89.9;
	}
	render->camera.rotation.x = nfmod(render->camera.rotation.x, 360);

	render->camera.front.x = cos(glm::radians(render->camera.rotation.x)) * cos(glm::radians(render->camera.rotation.y));
	render->camera.front.y = sin(glm::radians(render->camera.rotation.y));
	render->camera.front.z = sin(glm::radians(render->camera.rotation.x)) * cos(glm::radians(render->camera.rotation.y));
	render->camera.front = glm::normalize(render->camera.front);

    render->view = glm::lookAt(render->camera.position, render->camera.position + render->camera.front, render->camera.up);

    render->projection = glm::perspectiveFov(glm::radians(90.0f), (float)getState()->settings.resolution.x, (float)getState()->settings.resolution.y , 0.1f, 1000.0f);

	for(passes::Pass *pass : gameState->renderPasses) {

		pass->draw(gameState);
	}

	static bool debug = false;
	if (input::keyCheck(GLFW_KEY_F1)) {

		print_d("Toggling debug render");

		debug = !debug;
		input::keySet(GLFW_KEY_F1, false);
	}
	if (input::keyCheck(GLFW_KEY_N)) {

		print_d("Toggling day/night");

		static bool night = false;
		if (night) {
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

			render->light.ambient = glm::vec3(0.02f, 0.02f, 0.02f) * color;
			render->light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * color;
			render->light.specular = glm::vec3(1.0f, 1.0f, 1.0f) * color;

			render->light.direction = glm::vec3(122.0f, -162.0f, -190.0f);

			night = false;
		} else {

			glm::vec3 color = glm::vec3(0.05f, 0.05f, 0.25f);

			render->light.ambient = glm::vec3(0.02f, 0.02f, 0.02f) * color;
			render->light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * color;
			render->light.specular = glm::vec3(1.0f, 1.0f, 1.0f) * color;

			render->light.direction = glm::vec3(-35.0f, -50.0f, 25.0f);

			night = true;
		}
		input::keySet(GLFW_KEY_N, false);
	}
	
	if (debug) {

		debugRender(gameState);
	} else {
		GLsizei width = (GLsizei)flare::getState()->settings.resolution.x;
		GLsizei height = (GLsizei)flare::getState()->settings.resolution.y;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gameState->renderPasses[gameState->renderPasses.size()-1]->fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, width, height,
				0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
}

float flare::render::getDeltaTime() {

    return getState()->render.deltaTime;
}

void flare::render::setShaderOutput(std::string textureName, uint textureId) {

	getState()->render.shaderOutput[textureName] = textureId;
}

uint flare::render::getShaderOutput(std::string textureName) {

	assert(getState()->render.shaderOutput.find(textureName) != getState()->render.shaderOutput.end());

	return getState()->render.shaderOutput[textureName];
}

void flare::render::quad() {

	// glBindVertexArray(vao);
	// glDrawArrays(GL_TRIANGLES, 0, 6);
	
	for (flare::asset::model::Mesh *mesh : getState()->render.quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indexSize, GL_UNSIGNED_INT, 0);
	}
}

void flare::render::setTexture(uint textureLocation, uint textureUniform) {

	glActiveTexture(GL_TEXTURE0 + textureLocation);
	glBindTexture(GL_TEXTURE_2D, textureUniform);
}
