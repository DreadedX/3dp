#include "flare/flare.h"

GLfloat vertices[] {
 // Position       Color              Texcoords
    -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, // top-left
     0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // top-right
     0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, //bottom-right
    -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f  //bottom-left
};

GLuint elements[] {
    0, 1, 2,
    2, 3, 0
};

GLuint uniColor;

void flare::render::init() {

    // Generating and binding vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generating vbo
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Binding vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Generating ebo
    GLuint ebo;
    glGenBuffers(1, &ebo);

    // Binding ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

void flare::render::update() {

    // TODO: This should eventually move
    static asset::TextureAsset *texture = asset::load<asset::TextureAsset>("base/logo");
    glBindTexture(GL_TEXTURE_2D, texture->id);
    static asset::ShaderAsset *shader = asset::load<asset::ShaderAsset>("base/test");
    glUseProgram(shader->id);

    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
