#include "flare/flare.h"

void flare::render::passes::Geometry::init() {

	shader = asset::load<asset::Shader>("base/geometry");

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	textures = new GLuint[GBUFFER_NUM_TEXTURES];
	glGenTextures(GBUFFER_NUM_TEXTURES, textures);
	glGenTextures(1, &depthTexture);

	for (uint i = 0; i < GBUFFER_NUM_TEXTURES; ++i) {

		glBindTexture(GL_TEXTURE_2D, textures[i]);

		if (i == GBUFFER_TEXTURE_TYPE_POSITION || i == GBUFFER_TEXTURE_TYPE_SPECULAR_LIGHT) {

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
		} else {

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		}

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
	}

	glBindTexture(GL_TEXTURE_2D,depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 }; 
	glDrawBuffers(GBUFFER_NUM_TEXTURES, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void flare::render::passes::Geometry::draw() {

	flare::render::setShader(shader);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	getState()->pass = flare::render::GEOMETRY;
	fuse::draw();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
