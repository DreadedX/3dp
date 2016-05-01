#include "flare/flare.h"

void geometryPass(flare::component::Object *object) {

	for (flare::asset::model::Mesh *mesh : object->model->meshes) {

		if (mesh->diffuse != nullptr) {

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->diffuse->id);
		}
		// if (mesh->material->normal != nullptr) {
		//
		//     glActiveTexture(GL_TEXTURE1);
		//     glBindTexture(GL_TEXTURE_2D, mesh->normal->id);
		// }
		if (mesh->specular != nullptr) {

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, mesh->specular->id);
		}

		glBindVertexArray(mesh->vao);

		flare::render::State *state = flare::render::getState();

		{
			glUniformMatrix4fv(flare::render::getState()->geometryShader->locations.view, 1, GL_FALSE, glm::value_ptr(state->view));
			glUniformMatrix4fv(flare::render::getState()->geometryShader->locations.projection, 1, GL_FALSE, glm::value_ptr(state->projection));
		}

		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(modelMatrix, object->position->position);
		if (object->rotation != nullptr) {

			modelMatrix = glm::rotate(modelMatrix, object->rotation->rotation, object->rotation->rotationAxis);  
		}
		glUniformMatrix4fv(flare::render::getState()->geometryShader->locations.model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void flare::component::Object::_draw() {

	switch (render::getState()->pass) {

		case render::GEOMETRY:
			geometryPass(this);
			break;
	}
}
