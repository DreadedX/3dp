#include "flare/flare.h"

/** @todo This should propably be in a config */
float shadowMapSize = 1024*2;
 
void flare::render::passes::Shadow::init() {

	shader = asset::load<asset::Shader>("core/shadow");

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Enables rendering the shadowmap to a texture for debugging
#if 1
	GLuint texture = 0;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, shadowMapSize, shadowMapSize, 0, GL_RED, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 }; 
	glDrawBuffers(1, drawBuffers);
#else
	glDrawBuffer(GL_NONE);
#endif

	GLuint depthTexture = 0;

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);


	textures.add(depthTexture);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			print::e("error");
		}
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void flare::render::passes::Shadow::draw(GameState *gameState) {

	flare::render::setShader(shader);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glViewport(0, 0, shadowMapSize, shadowMapSize);

	glDepthMask(GL_TRUE);
	glCullFace(GL_FRONT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	State::Render *render = &getState()->render;

	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-400, 400, -400, 400, -400, 400);
	glm::mat4 depthViewMatrix = glm::lookAt(-render->light.direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 depthModelMatrix = glm::mat4(1.0f);

	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

	glUniformMatrix4fv(render->shader->locations.depthMVP, 1, GL_FALSE, &depthMVP[0][0]);

	getState()->render.pass = State::Render::GEOMETRY;
	gameState->manager->draw();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, getState()->settings.resolution.x, getState()->settings.resolution.y);
}
