#include "flare/flare.h"

/** @brief Store the render state */
flare::render::State state;

void flare::render::init() {

	state.renderPasses.push_back(new passes::Geometry);
	state.renderPasses.push_back(new passes::SSAO);
	state.renderPasses.push_back(new passes::Lighting);

	for(passes::Pass *pass : state.renderPasses) {

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
    state.light.direction = glm::vec3(45.0f, -50.0f, 65.0f);

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f) * color;
    state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * color;
    state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f) * color;

    // state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f);
    // state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    // state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	
	/** @todo This needs to not use a model, but a hardcoded quad */
	state.quad = asset::load<asset::Model>("quad");
}

void debugRender() {

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, state.renderPasses[0]->fbo);

		// glReadBuffer(GL_COLOR_ATTACHMENT0);
		// glBlitFramebuffer(0, 0, getSettings()->resolution.x, getSettings()->resolution.y, 
		// 		0, 0, getSettings()->resolution.x, getSettings()->resolution.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		GLsizei width = (GLsizei)flare::getSettings()->resolution.x;
		GLsizei height = (GLsizei)flare::getSettings()->resolution.y;

		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, width, height, 
				0, height/2, width/2, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glReadBuffer(GL_COLOR_ATTACHMENT4);
		glBlitFramebuffer(0, 0, width, height, 
				width/2, height/2, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glBlitFramebuffer(0, 0, width, height, 
				width/2, 0, width, height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR); 

		glBindFramebuffer(GL_READ_FRAMEBUFFER, state.renderPasses[1]->fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, width, height,
				0, 0, width/2, height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

/** @todo Functions like these need unit testing */
float nfmod(float a, float b) {
	
	return a - b * floor(a / b);
}

void flare::render::update() {

	// Calculate the camera angle
	// state.camera->rotation.y = nfmod(state.camera->rotation.y, 360);
	if (state.camera->rotation.y > 89.9) {
		state.camera->rotation.y = 89.9;
	}
	if (state.camera->rotation.y < -89.9) {
		state.camera->rotation.y = -89.9;
	}
	state.camera->rotation.x = nfmod(state.camera->rotation.x, 360);

	state.camera->front.x = cos(glm::radians(state.camera->rotation.x)) * cos(glm::radians(state.camera->rotation.y));
	state.camera->front.y = sin(glm::radians(state.camera->rotation.y));
	state.camera->front.z = sin(glm::radians(state.camera->rotation.x)) * cos(glm::radians(state.camera->rotation.y));
	state.camera->front = glm::normalize(state.camera->front);

    state.view = glm::lookAt(state.camera->position, state.camera->position + state.camera->front, state.camera->up);

    state.projection = glm::perspectiveFov(glm::radians(90.0f), (float)getSettings()->resolution.x, (float)getSettings()->resolution.y , 0.1f, 1000.0f);

	for(passes::Pass *pass : state.renderPasses) {

		pass->draw();
	}

	static bool debug = false;
	if (input::keyCheck(GLFW_KEY_F1)) {

		print::d("Toggling debug render");

		debug = !debug;
		input::keySet(GLFW_KEY_F1, false);
	}
	static bool night = false;
	if (input::keyCheck(GLFW_KEY_N)) {

		print::d("Toggling day/night");

		if (night) {
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

			state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f) * color;
			state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * color;
			state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f) * color;

			state.light.direction = glm::vec3(45.0f, -50.0f, 65.0f);

			night = false;
		} else {

			glm::vec3 color = glm::vec3(0.05f, 0.05f, 0.25f);

			state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f) * color;
			state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * color;
			state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f) * color;

			state.light.direction = glm::vec3(-35.0f, -50.0f, 25.0f);

			night = true;
		}
		input::keySet(GLFW_KEY_N, false);
	}
	
	if (debug) {

		debugRender();
	} else {
		GLsizei width = (GLsizei)flare::getSettings()->resolution.x;
		GLsizei height = (GLsizei)flare::getSettings()->resolution.y;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, state.renderPasses[state.renderPasses.size()-1]->fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, width, height,
				0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
}

void flare::render::setShader(flare::asset::Shader *shader) {

    if (state.shader == nullptr || state.shader->id != shader->id) {

		glUseProgram(shader->id);
		glUniform1i(glGetUniformLocation(shader->id, "material.diffuse"), 0);
		glUniform1i(glGetUniformLocation(shader->id, "material.normal"), 1);
		glUniform1i(glGetUniformLocation(shader->id, "material.specular"), 2);

		glUniform1i(glGetUniformLocation(shader->id, "gPositionMap"), 0);
		glUniform1i(glGetUniformLocation(shader->id, "gColorMap"), 1);
		glUniform1i(glGetUniformLocation(shader->id, "gNormalMap"), 2);
		glUniform1i(glGetUniformLocation(shader->id, "gTexCoordMap"), 3);
		glUniform1i(glGetUniformLocation(shader->id, "gDiffuseColorMap"), 4);
		glUniform1i(glGetUniformLocation(shader->id, "gSpecularColorMap"), 5);
		glUniform1i(glGetUniformLocation(shader->id, "ssaoBlurTexture"), 6);

		glUniform1i(glGetUniformLocation(shader->id, "ssaoTexture"), 0);

		glUniform1i(glGetUniformLocation(shader->id, "noiseTexture"), 1);
		state.shader = shader;
	
    }
}

flare::render::State *flare::render::getState() {
    return &state;
}

float flare::render::getDeltaTime() {
    return state.deltaTime;
}

