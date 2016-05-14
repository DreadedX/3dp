#ifndef GAUSIAN_H
#define GAUSIAN_H

struct Gausian : flare::render::passes::Pass {

	GLfloat test = 0.0;

	flare::asset::Shader *shaderOne = nullptr;
	GLuint fboOne;

	void init() override;
	void draw(flare::GameState *gameState) override;
};

#endif

