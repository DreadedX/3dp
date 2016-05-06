#include "flare/flare.h"

void flare::render::passes::Lighting::init() {

	shader = asset::load<asset::Shader>("base/lighting");

	GLuint lightingTexture = 0;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &lightingTexture);
	glBindTexture(GL_TEXTURE_2D, lightingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightingTexture, 0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	textures.add(lightingTexture);
}

void flare::render::passes::Lighting::draw() {
	
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	for (unsigned int i = 0 ; i < Geometry::GBUFFER_NUM_TEXTURES; i++) {

		glActiveTexture(GL_TEXTURE0 + i);	
		glBindTexture(GL_TEXTURE_2D, getState()->renderPasses[0]->textures[i]);
	}

	glActiveTexture(GL_TEXTURE0 + Geometry::GBUFFER_NUM_TEXTURES);	
	glBindTexture(GL_TEXTURE_2D, getState()->renderPasses[1]->textures[1]);

	glClear(GL_COLOR_BUFFER_BIT);

	flare::render::setShader(shader);

		{
			glUniform3fv(getState()->shader->locations.light.direction, 1, glm::value_ptr(getState()->light.direction));
			glUniform3fv(getState()->shader->locations.light.ambient, 1, glm::value_ptr(getState()->light.ambient));

			glUniform3fv(getState()->shader->locations.viewPosition, 1, glm::value_ptr(getState()->camera->position));
		}

		glUniformMatrix4fv(flare::render::getState()->shader->locations.model, 1, GL_FALSE, glm::value_ptr(glm::mat4()));

	for (flare::asset::model::Mesh *mesh : getState()->quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
