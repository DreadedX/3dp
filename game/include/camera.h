#ifndef CAMERA_H
#define CAMERA_H

#include "fuse/component.h"

/** @brief Component that allows you to move around with wasd and have mouselook
  @todo This should not be part of the game engine */
struct Camera : fuse::Component {

	/** @brief Update camera logic */
	void _update() override;

	/** Camera Component constructor */
	Camera(fuse::Entity *parent) {
		super(parent);
	}
};

struct JoyCamera : fuse::Component {

	void _update() override;

	JoyCamera(fuse::Entity *parent) {
		super(parent);

		// TESTING
#if 0
#endif
	}

};

#endif

