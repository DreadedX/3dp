#ifndef POSTFX_H
#define POSTFX_H

struct PostFX : flare::render::passes::Pass {

	PostFX(std::string name) {

		this->name = name;
		previous = flare::getState()->render.renderPasses.size() - 1;
	}

	std::string name;
	size_t previous = 0;

	GLfloat test = 0.0;

	void init() override;
	void draw() override;
};

#endif

