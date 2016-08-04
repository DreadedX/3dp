#include "flare/flare.h"

void flare::component::Object::_draw() {

	for (asset::model::Mesh *mesh : model->meshes) {

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

		State::Render *render = &getState()->render;
		glUniform3fv(render->shader->getLocation("light.diffuse"), 1, glm::value_ptr(mesh->diffuseColor * render->light.diffuse));		
		glUniform3fv(render->shader->getLocation("light.specular"), 1, glm::value_ptr(mesh->specularColor * render->light.specular));		

		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(modelMatrix, parent->getAttribute<glm::vec3>("position"));
		if (parent->hasAttribute("scale")) {

			modelMatrix = glm::scale(modelMatrix, parent->getAttribute<glm::vec3>("scale"));
		}
		if (parent->hasAttribute("rotation") && parent->hasAttribute("rotationAxis")) {

			modelMatrix = glm::rotate(modelMatrix, parent->getAttribute<float>("rotation"), parent->getAttribute<glm::vec3>("rotationAxis"));  
		}
		glUniformMatrix4fv(render->shader->getLocation("model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

		glDrawElements(GL_TRIANGLES, mesh->indexSize, GL_UNSIGNED_INT, 0);
	}
}
