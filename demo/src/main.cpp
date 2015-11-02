#include "standard.h"

int main() {

    std::cout << "This is a demo program for the flare engine." << std::endl;

    flare::init();

    while (flare::isRunning()) {

	flare::update();
    }

    flare::terminate();
}


