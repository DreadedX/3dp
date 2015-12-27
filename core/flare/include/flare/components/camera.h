#ifndef CAMERA_H
#define CAMERA_H

namespace flare {
    namespace component {

	struct Camera : fuse::Component {

	    void _update() override;

	    Camera(fuse::Entity *parent) {
		super(parent);
	    }
	};
    }
}

#endif

