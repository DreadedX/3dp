#include "flare/flare.h"

void flare::render::init() {

	State::Render *render = &getState()->render;

	render->renderPasses.add(new passes::Geometry);
	render->renderPasses.add(new passes::Skybox);
	render->renderPasses.add(new passes::SSAO);
	render->renderPasses.add(new passes::Lighting);

	for(passes::Pass *pass : render->renderPasses) {

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
	render->quad = asset::load<asset::Model>("core/quad");
}

void debugRender() {

		flare::State::Render *render = &flare::getState()->render;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLsizei width = (GLsizei)flare::getState()->settings.resolution.x;
		GLsizei height = (GLsizei)flare::getState()->settings.resolution.y;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, render->renderPasses[flare::State::Render::GEOMETRY]->fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, width, height, 
				0, height/2, width/2, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glReadBuffer(GL_COLOR_ATTACHMENT4);
		glBlitFramebuffer(0, 0, width, height, 
				width/2, height/2, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glBlitFramebuffer(0, 0, width, height, 
				width/2, 0, width, height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR); 

		glBindFramebuffer(GL_READ_FRAMEBUFFER, render->renderPasses[flare::State::Render::SKYBOX]->fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, width, height,
				0, 0, width/2, height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

/** @todo Functions like these need unit testing */
float nfmod(float a, float b) {
	
	return a - b * floor(a / b);
}

void flare::render::update() {

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

	for(passes::Pass *pass : render->renderPasses) {

		pass->draw();
	}

	static bool debug = false;
	if (input::keyCheck(GLFW_KEY_F1)) {

		print::d("Toggling debug render");

		debug = !debug;
		input::keySet(GLFW_KEY_F1, false);
	}
	if (input::keyCheck(GLFW_KEY_N)) {

		print::d("Toggling day/night");

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

		debugRender();
	} else {
		GLsizei width = (GLsizei)flare::getState()->settings.resolution.x;
		GLsizei height = (GLsizei)flare::getState()->settings.resolution.y;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, render->renderPasses[render->renderPasses.size()-1]->fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, width, height,
				0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
}

void flare::render::setShader(flare::asset::Shader *shader) {

	State::Render *render = &getState()->render;

    if (render->shader == nullptr || render->shader->id != shader->id) {

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
		glUniform1i(glGetUniformLocation(shader->id, "skyboxTexture"), 7);

		glUniform1i(glGetUniformLocation(shader->id, "ssaoTexture"), 0);
		glUniform1i(glGetUniformLocation(shader->id, "noiseTexture"), 1);

		glUniform1i(glGetUniformLocation(shader->id, "skybox"), 0);
		render->shader = shader;
	
    }
}

float flare::render::getDeltaTime() {

    return getState()->render.deltaTime;
}

