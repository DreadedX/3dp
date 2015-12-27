#ifndef OBJECT_H
#define OBJECT_H

namespace flare {
    namespace component {

	struct Object : fuse::Component {

	    Position *position = nullptr;
	    Rotation *rotation = nullptr;

	    asset::Model *model = nullptr;
	    asset::Shader *shader = nullptr;
	    asset::Material *material = nullptr;
	    asset::Texture *texture = nullptr;

	    void _draw() override;

	    Object(fuse::Entity *parent, std::string model, std::string shader, std::string material) {
		super(parent);

		this->position = parent->getComponent<Position>();
		if (parent->hasComponent<Rotation>()) {

		    this->rotation = parent->getComponent<Rotation>();
		}

		this->model = asset::load<asset::Model>(model);
		this->shader = asset::load<asset::Shader>(shader);
		this->material = asset::load<asset::Material>(material);
	    }
	};
    }
}

#endif

