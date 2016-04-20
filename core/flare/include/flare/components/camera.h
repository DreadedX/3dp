#ifndef CAMERA_H
#define CAMERA_H

namespace flare {
	namespace component {

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
	}
}

#endif

