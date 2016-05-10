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

		flare::State::Render *render = &flare::getState()->render;

		{
			glUniform3fv(render->shader->locations.light.diffuse, 1, glm::value_ptr(mesh->diffuseColor * render->light.diffuse));		
			glUniform3fv(render->shader->locations.light.specular, 1, glm::value_ptr(mesh->specularColor * render->light.specular));		

			glUniformMatrix4fv(render->shader->locations.view, 1, GL_FALSE, glm::value_ptr(render->view));
			glUniformMatrix4fv(render->shader->locations.projection, 1, GL_FALSE, glm::value_ptr(render->projection));
		}

		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(modelMatrix, object->position->position);
		if (object->rotation != nullptr) {

			modelMatrix = glm::rotate(modelMatrix, object->rotation->rotation, object->rotation->rotationAxis);  
		}
		glUniformMatrix4fv(render->shader->locations.model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		glDrawElements(GL_TRIANGLES, mesh->indexSize, GL_UNSIGNED_INT, 0);
	}
}

void flare::component::Object::_draw() {

	switch (getState()->render.pass) {

		case State::Render::GEOMETRY:
			geometryPass(this);
			break;
	}
}
