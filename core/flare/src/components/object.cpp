#include "flare/flare.h"

void flare::component::Object::_draw() {

    // model->draw(shader, material, this);
    render::setShader(shader);
    render::setMaterial(material);

    for (asset::model::Mesh *mesh : model->meshes) {

	glBindVertexArray(mesh->vao);
	// glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// glBindBuffer(GL_ARRAY_BUFFER, ebo);

	// print::d("%f, %f, %f", vertices[0].position.x, vertices[0].position.y, vertices[0].position.z);
	// print::d("%s", shader->name.c_str());

	flare::render::State *state = flare::render::getState();

	{
	    glUniformMatrix4fv(shader->locations.view, 1, GL_FALSE, glm::value_ptr(state->view));
	    glUniformMatrix4fv(shader->locations.projection, 1, GL_FALSE, glm::value_ptr(state->projection));

	    glUniform3fv(shader->locations.light.position, 1, glm::value_ptr(state->light.position));
	    glUniform3fv(shader->locations.light.ambient, 1, glm::value_ptr(state->light.ambient));
	    glUniform3fv(shader->locations.light.diffuse, 1, glm::value_ptr(state->light.diffuse));
	    glUniform3fv(shader->locations.light.specular, 1, glm::value_ptr(state->light.specular));

	    glUniform3fv(shader->locations.viewPosition, 1, glm::value_ptr(render::getCamera()->position));

	    glUniform1f(shader->locations.material.shininess, material->shininess);
	}

	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, position->position);
	if (rotation != nullptr) {

	    modelMatrix = glm::rotate(modelMatrix, rotation->rotation, rotation->rotationAxis);  
	}
	glUniformMatrix4fv(shader->locations.model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    }
}
