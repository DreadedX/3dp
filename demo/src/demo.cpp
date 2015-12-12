#include "standard.h"

int main() {

    flare::init();

    flare::log::l("This is a demo program for the flare engine!");

    while (flare::isRunning()) {

	flare::update();
    }

    flare::terminate();
}
