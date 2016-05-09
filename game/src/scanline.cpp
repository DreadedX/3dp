#include "demo.h"

void Scanline::init() {

	shader = flare::asset::load<flare::asset::Shader>("demo/scanline");

	GLuint texture = 0;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flare::getState()->settings.resolution.x, flare::getState()->settings.resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	textures.add(texture);
}

void Scanline::draw() {

	flare::State::Render *render = &flare::getState()->render;
	
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, render->renderPasses[2]->textures[0]);

	glClear(GL_COLOR_BUFFER_BIT);

	flare::render::setShader(shader);

		{
			glUniform3fv(render->shader->locations.light.direction, 1, glm::value_ptr(render->light.direction));
			glUniform3fv(render->shader->locations.light.ambient, 1, glm::value_ptr(render->light.ambient));

			glUniform3fv(render->shader->locations.viewPosition, 1, glm::value_ptr(render->camera.position));
		}

		glUniformMatrix4fv(render->shader->locations.model, 1, GL_FALSE, glm::value_ptr(glm::mat4()));

	for (flare::asset::model::Mesh *mesh : render->quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
