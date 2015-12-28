#include "flare/flare.h"

void flare::component::Object::_draw() {

    render::setShader(shader);

    for (asset::model::Mesh *mesh : model->meshes) {

	if (mesh->diffuse != nullptr) {

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, mesh->diffuse->id);
	}
	if (mesh->normal != nullptr) {

	    glActiveTexture(GL_TEXTURE1);
	    glBindTexture(GL_TEXTURE_2D, mesh->normal->id);
	}
	if (mesh->specular != nullptr) {

	    glActiveTexture(GL_TEXTURE2);
	    glBindTexture(GL_TEXTURE_2D, mesh->specular->id);
	}

	glBindVertexArray(mesh->vao);

	flare::render::State *state = flare::render::getState();

	{
	    glUniformMatrix4fv(shader->locations.view, 1, GL_FALSE, glm::value_ptr(state->view));
	    glUniformMatrix4fv(shader->locations.projection, 1, GL_FALSE, glm::value_ptr(state->projection));

	    glUniform3fv(shader->locations.light.direction, 1, glm::value_ptr(state->light.direction));
	    glUniform3fv(shader->locations.light.ambient, 1, glm::value_ptr(state->light.ambient));
	    glUniform3fv(shader->locations.light.diffuse, 1, glm::value_ptr(state->light.diffuse));
	    glUniform3fv(shader->locations.light.specular, 1, glm::value_ptr(state->light.specular));

	    glUniform3fv(shader->locations.viewPosition, 1, glm::value_ptr(render::getCamera()->position));

	    // TODO: This should also be loaded from the obj
	    glUniform1f(shader->locations.material.shininess, 16);
	}

	static bool toggle = true;
	if (input::keyCheck(GLFW_KEY_T)) {

	    if (toggle) {
		toggle = false;
	    } else {
		toggle = true;
	    }
	    input::keySet(GLFW_KEY_T, false);
	}
	glUniform1i(shader->locations.toggle, toggle ? 1 : 0);


	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, position->position);
	if (rotation != nullptr) {

	    modelMatrix = glm::rotate(modelMatrix, rotation->rotation, rotation->rotationAxis);  
	}
	glUniformMatrix4fv(shader->locations.model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    }
}
