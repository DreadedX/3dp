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

// TODO: Make sure this code gets cleaned up properly
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

    // TODO: This needs to be moved out of here
    asset::ShaderAsset *shader = asset::load<asset::ShaderAsset>("base/test");
    glUseProgram(shader->id);
    
    GLint posAttrib = glGetAttribLocation(shader->id, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);

    GLint colAttrib = glGetAttribLocation(shader->id, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));

    GLint texAttrib = glGetAttribLocation(shader->id, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));

    // TODO: This is just for testing
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
}

void flare::render::update() {

    // TODO: This should eventually move
    static asset::TextureAsset *texture = asset::load<asset::TextureAsset>("base/logo");
    glBindTexture(GL_TEXTURE_2D, texture->id);

    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
