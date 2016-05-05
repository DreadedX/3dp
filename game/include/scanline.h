#ifndef SCANLINE_H
#define SCANLINE_H

struct Scanline : flare::render::passes::Pass {

	void init() override;
	void draw() override;
};

#endif

