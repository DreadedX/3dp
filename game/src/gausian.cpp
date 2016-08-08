#include <GL/glew.h>
#include <GL/gl.h>

#include "glm/gtc/type_ptr.hpp"

#include "flare/engine.h"

#include "gausian.h"

void Gausian::init() {

	shaderOne = flare::asset::load<flare::asset::Shader>("demo/shader/gausian");

	GLuint texture = 0;

	glGenFramebuffers(1, &fboOne);
	glBindFramebuffer(GL_FRAMEBUFFER, fboOne);
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

	shader = flare::asset::load<flare::asset::Shader>("demo/shader/gausian2");

	texture = 0;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flare::getState()->settings.resolution.x, flare::getState()->settings.resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print_e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	textures.add(texture);
}

void Gausian::draw(flare::GameState *gameState) {

	flare::State::Render *render = &flare::getState()->render;
	
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindFramebuffer(GL_FRAMEBUFFER, fboOne);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, dynamic_cast<flare::PauseState*>(gameState)->previousState->renderPasses[dynamic_cast<flare::PauseState*>(gameState)->previousState->renderPasses.size()-1]->textures[0]);

	glClear(GL_COLOR_BUFFER_BIT);

	shaderOne->use();

		{
			glUniform3fv(render->shader->locations["light.direction"], 1, glm::value_ptr(render->light.direction));
			glUniform3fv(render->shader->locations["light.ambient"], 1, glm::value_ptr(render->light.ambient));

			glUniform3fv(render->shader->locations["viewPosition"], 1, glm::value_ptr(render->camera.position));
		}

		glUniformMatrix4fv(render->shader->locations["model"], 1, GL_FALSE, glm::value_ptr(glm::mat4()));

		/** @todo Change the variable to something else than test */
		glUniform1f(glGetUniformLocation(shader->id, "test"), test);

	for (flare::asset::model::Mesh *mesh : render->quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indexSize, GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glActiveTexture(GL_TEXTURE0);

	/** @todo Need to make this so that it takes the output of the previous shader */
	glBindTexture(GL_TEXTURE_2D, gameState->renderPasses[0]->textures[0]);
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
