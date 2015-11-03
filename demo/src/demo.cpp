#include "standard.h"

int main() {

    flare::init();

    printf("This is a demo program for the flare engine!\n");

    while (flare::isRunning()) {

	flare::update();
    }

    flare::terminate();
}


