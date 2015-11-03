#include "flare/flare.h"

GLfloat vertices[] {
    0.0f, 0.5f,
    0.5f, -0.5f,
    -0.5f, -0.5f
};

// TODO: Make sure this code gets cleaned up properly
void flare::initRender() {

    // Generating buffer
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Binding buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // TODO: This needs to be moved out of here
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint shaderProgram = flare::loadShader();
    glUseProgram(shaderProgram);
    
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void flare::updateRender() {

    GLuint error = glGetError();
    if (error != 0) {

	printf("OpenGL error while rendering: %i\n", error);
    }

    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
