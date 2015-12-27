#include "flare/flare.h"

void flare::component::Object::_draw() {

    model->draw(shader, material, this);
}
