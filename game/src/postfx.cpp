#include <GL/glew.h>

#include "glm/gtc/type_ptr.hpp"

#include "flare/engine.h"

#include "postfx.h"

void PostFX::init() {

	shader = flare::asset::load<flare::asset::Shader>(name);

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
		print_e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	textures.add(texture);
}

void PostFX::draw(flare::GameState *gameState) {

	flare::State::Render *render = &flare::getState()->render;
	
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glActiveTexture(GL_TEXTURE0);

	/** @todo Need to make this so that it takes the output of the previous shader */
	glBindTexture(GL_TEXTURE_2D, gameState->renderPasses[previous]->textures[0]);
	// This is just a hack to test out some things

	glClear(GL_COLOR_BUFFER_BIT);

	shader->use();

		{
			glUniform3fv(render->shader->locations["light.direction"], 1, glm::value_ptr(render->light.direction));
			glUniform3fv(render->shader->locations["light.ambient"], 1, glm::value_ptr(render->light.ambient));

			glUniform3fv(render->shader->locations["viewPosition"], 1, glm::value_ptr(render->camera.position));
		}

		glUniformMatrix4fv(render->shader->locations["model"], 1, GL_FALSE, glm::value_ptr(glm::mat4()));

		glUniform1f(glGetUniformLocation(shader->id, "test"), test);

	for (flare::asset::model::Mesh *mesh : render->quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indexSize, GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
