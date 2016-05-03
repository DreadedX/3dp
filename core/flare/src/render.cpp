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

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) {

	return a + f * (b - a);
}

std::vector<glm::vec3> ssaoKernel;

void generateSSAOKernel() {
	
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	for (GLuint i = 0; i < 64; i++) {

		glm::vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);

		GLfloat scale = GLfloat(i) / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	std::vector<glm::vec3> ssaoNoise;
	for (GLuint i = 0; i < 16; i++) {

		glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f
			);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &state.ssaoNoise);
	glBindTexture(GL_TEXTURE_2D, state.ssaoNoise);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void flare::render::init() {

	state.geometryShader = asset::load<asset::Shader>("base/geometry");
	state.ssaoShader = asset::load<asset::Shader>("base/ssao");
	state.ssaoBlurShader = asset::load<asset::Shader>("base/ssaoBlur");
	state.lightingShader = asset::load<asset::Shader>("base/lighting");

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
    state.light.direction = glm::vec3(45.0f, -50.0f, 65.0f);

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f) * color;
    state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * color;
    state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f) * color;

    // state.light.ambient = glm::vec3(0.02f, 0.02f, 0.02f);
    // state.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    // state.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	
	// Setup for geometry pass
	glGenFramebuffers(1, &state.geometryFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.geometryFbo);

	glGenTextures(GBUFFER_NUM_TEXTURES, state.geometryTextures);
	glGenTextures(1, &state.depthTexture);

	for (uint i = 0; i < GBUFFER_NUM_TEXTURES; ++i) {

		glBindTexture(GL_TEXTURE_2D, state.geometryTextures[i]);

		if (i == GBUFFER_TEXTURE_TYPE_POSITION || i == GBUFFER_TEXTURE_TYPE_SPECULAR_LIGHT) {

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
		} else {

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		}

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, state.geometryTextures[i], 0);
	}
    
	glBindTexture(GL_TEXTURE_2D, state.depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, state.depthTexture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 }; 
	glDrawBuffers(GBUFFER_NUM_TEXTURES, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setup for SSAO
	glGenFramebuffers(1, &state.ssaoFbo);  
	glBindFramebuffer(GL_FRAMEBUFFER, state.ssaoFbo);

	glGenTextures(1, &state.ssaoTexture);
	glBindTexture(GL_TEXTURE_2D, state.ssaoTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state.ssaoTexture, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glGenFramebuffers(1, &state.ssaoBlurFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, state.ssaoBlurFbo);
	glGenTextures(1, &state.ssaoBlurTexture);
	glBindTexture(GL_TEXTURE_2D, state.ssaoBlurTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state.ssaoBlurTexture, 0);

	generateSSAOKernel();

	/** @todo This needs to not use a model, but a hardcoded quad */
	state.quad = asset::load<asset::Model>("quad");
}

void debugRender() {

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, state.geometryFbo);

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

		glBindFramebuffer(GL_READ_FRAMEBUFFER, state.ssaoBlurFbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, width, height,
				0, 0, width/2, height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

void geometryPass() {

	flare::render::setShader(state.geometryShader);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.geometryFbo);

	glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	state.pass = flare::render::GEOMETRY;
	fuse::draw();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
}

void ssaoPass() {

	// SSAO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.ssaoFbo);

    glClear(GL_COLOR_BUFFER_BIT);

	flare::render::setShader(state.ssaoShader);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state.geometryTextures[flare::render::GBUFFER_TEXTURE_TYPE_POSITION]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, state.ssaoNoise);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, state.geometryTextures[flare::render::GBUFFER_TEXTURE_TYPE_NORMAL]);

	for (GLuint i = 0; i < 64; ++i) {

		glUniform3fv(glGetUniformLocation(state.shader, ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);
	}

	glUniformMatrix4fv(state.ssaoShader->locations.view, 1, GL_FALSE, glm::value_ptr(state.view));
	glUniformMatrix4fv(state.ssaoShader->locations.projection, 1, GL_FALSE, glm::value_ptr(state.projection));

	for (flare::asset::model::Mesh *mesh : state.quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}

	// Blur
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.ssaoBlurFbo);

    glClear(GL_COLOR_BUFFER_BIT);

	flare::render::setShader(state.ssaoBlurShader);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state.ssaoTexture);

	for (flare::asset::model::Mesh *mesh : state.quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void lightingPass() {

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	for (unsigned int i = 0 ; i < flare::render::GBUFFER_NUM_TEXTURES; i++) {

		glActiveTexture(GL_TEXTURE0 + i);	
		glBindTexture(GL_TEXTURE_2D, state.geometryTextures[i]);
	}

	glActiveTexture(GL_TEXTURE0 + flare::render::GBUFFER_NUM_TEXTURES);	
	glBindTexture(GL_TEXTURE_2D, state.ssaoBlurTexture);

	glClear(GL_COLOR_BUFFER_BIT);

	flare::render::setShader(state.lightingShader);

		{
			glUniform3fv(state.lightingShader->locations.light.direction, 1, glm::value_ptr(state.light.direction));
			glUniform3fv(state.lightingShader->locations.light.ambient, 1, glm::value_ptr(state.light.ambient));

			glUniform3fv(state.lightingShader->locations.viewPosition, 1, glm::value_ptr(flare::render::getCamera()->position));
		}

		glUniformMatrix4fv(flare::render::getState()->lightingShader->locations.model, 1, GL_FALSE, glm::value_ptr(glm::mat4()));

	for (flare::asset::model::Mesh *mesh : state.quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void postPass() {

}

void flare::render::update() {

    state.view = glm::lookAt(state.camera.position, state.camera.position + state.camera.front, state.camera.up);

    state.projection = glm::perspectiveFov(glm::radians(90.0f), (float)getSettings()->resolution.x, (float)getSettings()->resolution.y , 0.1f, 1000.0f);

	geometryPass();

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
	
	ssaoPass();

	if (debug) {

		debugRender();
		return;
	}

	lightingPass();
	postPass();
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
		glUniform1i(glGetUniformLocation(shader->id, "gDiffuseColorMap"), 4);
		glUniform1i(glGetUniformLocation(shader->id, "gSpecularColorMap"), 5);
		glUniform1i(glGetUniformLocation(shader->id, "ssaoBlurTexture"), 6);

		glUniform1i(glGetUniformLocation(shader->id, "ssaoTexture"), 0);

		glUniform1i(glGetUniformLocation(shader->id, "noiseTexture"), 1);
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

