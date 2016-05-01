#include "flare/flare.h"

/** @brief Store the render state */
flare::render::State state;

GLfloat quadVertices[] = {
        // Left bottom triangle
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        // Right top triangle
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
};

void flare::render::init() {

	state.geometryShader = asset::load<asset::Shader>("base/geometry");
	state.directionalShader = asset::load<asset::Shader>("base/directional");

	setShader(state.geometryShader);

    // Enable MSAA
    // glEnable(GL_MULTISAMPLE);

    // Enable depth testing
    // glEnable(GL_DEPTH_TEST);

	// Enable transparency
    // glEnable(GL_BLEND);
	// glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // glm::vec3(1.0f, 0.9f, 0.7f);
    // state.light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    state.light.direction = glm::vec3(50.0f, -50.0f, 50.0f);

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f) * color;
    state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * color;
    state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f) * color;

    // state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f);
    // state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    // state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	
	// Setup for geometry pass
	glGenFramebuffers(1, &state.fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.fbo);

	glGenTextures(GBUFFER_NUM_TEXTURES, state.textures);
	glGenTextures(1, &state.depthTexture);

	int samples = 4;

	// glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
	print::d("Max samples: %i", samples);

	for (uint i = 0; i < GBUFFER_NUM_TEXTURES; ++i) {

		glBindTexture(GL_TEXTURE_2D, state.textures[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGB, GL_FLOAT, NULL);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, state.textures[i], 0);
	}
    
	glBindTexture(GL_TEXTURE_2D, state.depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, state.depthTexture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 }; 
	glDrawBuffers(GBUFFER_NUM_TEXTURES, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	state.quad = asset::load<asset::Model>("quad");
}

void debugRender() {

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, state.fbo);

		// glReadBuffer(GL_COLOR_ATTACHMENT0);
		// glBlitFramebuffer(0, 0, getSettings()->resolution.x, getSettings()->resolution.y, 
		// 		0, 0, getSettings()->resolution.x, getSettings()->resolution.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		GLsizei width = (GLsizei)flare::getSettings()->resolution.x;
		GLsizei height = (GLsizei)flare::getSettings()->resolution.y;

		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glBlitFramebuffer(0, 0, width, height,
				0, 0, width/2, height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, width, height, 
				0, height/2, width/2, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glReadBuffer(GL_COLOR_ATTACHMENT5);
		glBlitFramebuffer(0, 0, width, height, 
				width/2, height/2, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glReadBuffer(GL_COLOR_ATTACHMENT6);
		glBlitFramebuffer(0, 0, width, height, 
				width/2, 0, width, height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR); 
}

void geometryPass() {

	flare::render::setShader(state.geometryShader);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.fbo);

	glDepthMask(GL_TRUE);

    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	state.pass = flare::render::GEOMETRY;
	fuse::draw();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
}

void beginLightPass() {

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	for (unsigned int i = 0 ; i < flare::render::GBUFFER_NUM_TEXTURES; i++) {

		glActiveTexture(GL_TEXTURE0 + i);	
		glBindTexture(GL_TEXTURE_2D, state.textures[i]);
	}

	glClear(GL_COLOR_BUFFER_BIT);
}

glm::mat4 identity = glm::mat4(1.0f);
void directionalLightPass() {

	flare::render::setShader(state.directionalShader);

	for (flare::asset::model::Mesh *mesh : state.quad->meshes) {

		glBindVertexArray(mesh->vao);

		flare::render::State *state = flare::render::getState();

		{
			glUniformMatrix4fv(state->directionalShader->locations.view, 1, GL_FALSE, glm::value_ptr(state->view));
			glUniformMatrix4fv(state->directionalShader->locations.projection, 1, GL_FALSE, glm::value_ptr(state->projection));

			glUniform3fv(state->directionalShader->locations.light.direction, 1, glm::value_ptr(state->light.direction));
			glUniform3fv(state->directionalShader->locations.light.ambient, 1, glm::value_ptr(state->light.ambient));

			glUniform3fv(state->directionalShader->locations.viewPosition, 1, glm::value_ptr(flare::render::getCamera()->position));

			glUniform1f(state->directionalShader->locations.material.shininess, mesh->shininess);
		}

		glm::mat4 modelMatrix = glm::mat4();
		// modelMatrix = glm::translate(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(flare::render::getState()->directionalShader->locations.model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void flare::render::update() {

    state.view = glm::lookAt(state.camera.position, state.camera.position + state.camera.front, state.camera.up);

    state.projection = glm::perspectiveFov(glm::radians(90.0f), (float)getSettings()->resolution.x, (float)getSettings()->resolution.y , 0.1f, 1000.0f);

	geometryPass();

	static bool toggle = true;
	if (input::keyCheck(GLFW_KEY_F1)) {

		print::d("Toggling debug render");

		toggle = !toggle;
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

			state.light.direction = glm::vec3(50.0f, -50.0f, 50.0f);

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

	if (toggle) {

		beginLightPass();
		directionalLightPass();
	} else {

		debugRender();
	}
}

void flare::render::setShader(flare::asset::Shader *shader) {

    if (state.shader != shader->id) {

	glUseProgram(shader->id);
	glUniform1i(glGetUniformLocation(shader->id, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(shader->id, "material.normal"), 1);
	glUniform1i(glGetUniformLocation(shader->id, "material.specular"), 2);

	glUniform1i(glGetUniformLocation(shader->id, "gPositionMap"), 0);
	glUniform1i(glGetUniformLocation(shader->id, "gColorMap"), 1);
	glUniform1i(glGetUniformLocation(shader->id, "gNormalMap"), 2);
	glUniform1i(glGetUniformLocation(shader->id, "gTexCoordMap"), 3);
	glUniform1i(glGetUniformLocation(shader->id, "gMaterialMap"), 4);
	glUniform1i(glGetUniformLocation(shader->id, "gDiffuseColorMap"), 5);
	glUniform1i(glGetUniformLocation(shader->id, "gSpecularColorMap"), 6);
	// glUniform1i(glGetUniformLocation(shader->id, "material.emission"), 3);
	state.shader = shader->id;
	
    }
}

flare::render::State *flare::render::getState() {
    return &state;
}

flare::render::Camera *flare::render::getCamera() {
    return &state.camera;
}

float flare::render::getDeltaTime() {
    return state.deltaTime;
}

