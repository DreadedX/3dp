#ifndef OBJECT_H
#define OBJECT_H

namespace flare {
    namespace component {

	struct Object : fuse::Component {

	    Position *position = nullptr;
	    Rotation *rotation = nullptr;

	    asset::Model *model = nullptr;
	    asset::Shader *shader = nullptr;

	    void _draw() override;

	    Object(fuse::Entity *parent, std::string model, std::string shader) {
		super(parent);

		this->position = parent->getComponent<Position>();
		if (parent->hasComponent<Rotation>()) {

		    this->rotation = parent->getComponent<Rotation>();
		}

		this->model = asset::load<asset::Model>(model);
		this->shader = asset::load<asset::Shader>(shader);
	    }
	};
    }
}

#endif

