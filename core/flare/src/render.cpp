#include "flare/flare.h"

GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f), 
  glm::vec3(-1.5f, -2.2f, -2.5f),  
  glm::vec3(-3.8f, -2.0f, -12.3f),  
  glm::vec3( 2.4f, -0.4f, -3.5f),  
  glm::vec3(-1.7f,  3.0f, -7.5f),  
  glm::vec3( 1.3f, -2.0f, -2.5f),  
  glm::vec3( 1.5f,  2.0f, -2.5f), 
  glm::vec3( 1.5f,  0.2f, -1.5f), 
  glm::vec3(-1.3f,  1.0f, -1.5f)  
};

// GLfloat vertices[] {
//  // Position       Color              Texcoords
//     -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, // top-left
//      0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // top-right
//      0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, //bottom-right
//     -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f  //bottom-left
// };

// GLuint elements[] {
//     0, 1, 2,
//     2, 3, 0
// };

flare::render::State state;

void flare::render::init() {
    //
    // Generating and binding vao for the lamp
    glGenVertexArrays(1, &state.vaoLamp);
    glBindVertexArray(state.vaoLamp);

    // Generating and binding vao for the object
    glGenVertexArrays(1, &state.vao);
    glBindVertexArray(state.vao);

    // TODO: This should move into model asset
    // Generating vbo
    glGenBuffers(1, &state.vbo);
    
    // Binding vbo
    glBindBuffer(GL_ARRAY_BUFFER, state.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // // Generating ebo
    // GLuint ebo;
    // glGenBuffers(1, &ebo);
    //
    // // Binding ebo
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    
    // Enable MSAA
    glEnable(GL_MULTISAMPLE);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

}

void flare::render::update() {

    glm::mat4 view;
    view = glm::lookAt(state.camera.posiion, state.camera.posiion + state.camera.front, state.camera.up);

    glm::mat4 projection;
    projection = glm::perspectiveFov(glm::radians(90.0f), (float)getSettings()->resolution.x, (float)getSettings()->resolution.y , 0.1f, 100.0f);

    glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.7f);
    glm::vec3 diffuseLocation = glm::vec3(25.0f, 50.0f, -25.0f);

    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, state.vbo);

    // Draw all the objects
    // TODO: Not all changes to shaders are hotswappable
    glBindVertexArray(state.vao);
    {
	// TODO: This should eventually move
	static asset::TextureAsset *texture = asset::load<asset::TextureAsset>("base/diamond_ore");
	glBindTexture(GL_TEXTURE_2D, texture->id);
	static asset::ShaderAsset *shader = asset::load<asset::ShaderAsset>("base/object");
	glUseProgram(shader->id);

	glUniformMatrix4fv(shader->locations.view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(shader->locations.projection, 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(shader->locations.lightColor, 1, glm::value_ptr(lightColor));
	glUniform3fv(shader->locations.diffuseLocation, 1, glm::value_ptr(diffuseLocation));
	glUniform3fv(shader->locations.specularLocation, 1, glm::value_ptr(state.camera.posiion));

	for (GLuint i = 0; i < 10; i++) {

	    glm::mat4 model;
	    model = glm::translate(model, cubePositions[i]);
	    model = glm::rotate(model, (GLfloat)glm::radians(20.0f * i), glm::vec3(0.5f, 1.0f, 0.0f));  
	    glUniformMatrix4fv(shader->locations.model, 1, GL_FALSE, glm::value_ptr(model));

	    glDrawArrays(GL_TRIANGLES, 0, 36);
	    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
    }

    glBindVertexArray(state.vaoLamp);
    {
	// TODO: This should eventually move
	static asset::ShaderAsset *shader = asset::load<asset::ShaderAsset>("base/lamp");
	glUseProgram(shader->id);

	glUniformMatrix4fv(shader->locations.view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(shader->locations.projection, 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(shader->locations.lightColor, 1, glm::value_ptr(lightColor));

	glm::mat4 model;
	model = glm::translate(model, diffuseLocation);
	glUniformMatrix4fv(shader->locations.model, 1, GL_FALSE, glm::value_ptr(model));

	glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

flare::render::State *flare::render::getState() {
    return &state;
}
flare::render::Camera *flare::render::getCamera() {
    return &state.camera;
}
float flare::render::getDeltaTime() {
    return state.deltaTime;
}

