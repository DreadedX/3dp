#ifndef POSTFX_H
#define POSTFX_H

#include "flare/game_state.h"
#include "flare/render.h"
#include "flare/passes.h"

struct PostFX : flare::render::passes::Pass {

	PostFX(std::string name, flare::GameState *gameState) {

		this->name = name;
		previous = gameState->renderPasses.size() - 1;
	}

	std::string name;
	size_t previous = 0;

	GLfloat test = 0.0;

	void init() override;
	void draw(flare::GameState *) override;
};

#endif

