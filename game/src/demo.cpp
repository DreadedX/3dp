#include "demo.h"

glm::vec3 cubePositions[] = {
	glm::vec3( 0.0f,  0.0f,  0.0f), 
	glm::vec3( 2.0f,  5.0f, -15.0f), 
	glm::vec3(-1.5f, -2.2f, -2.5f),  
	glm::vec3(-3.8f, -2.0f, -12.3f),  
	glm::vec3( 2.4f, -0.4f, -3.5f),  
	glm::vec3(-1.7f,  3.0f, -7.5f),  
	glm::vec3( 1.3f, -2.0f, -2.5f),  
	glm::vec3( 1.5f,  2.0f, -2.5f), 
	glm::vec3( 1.5f,  0.2f, -1.5f), 
	glm::vec3(-1.3f,  1.0f, -1.5f)  
};

struct JoyDebugControl : fuse::Component {

	JoyDebugControl(fuse::Entity *parent) {
		super(parent);
	}

	void _update() override {

		int countButtons;
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &countButtons);

		static bool reloaded = false;

		if (buttons[3] == 1 && !reloaded) {

			print::d("Reloading assets");
			flare::asset::reload();
		}

		if (buttons[3] == 0 && reloaded) {

			reloaded = false;
		}
	}
};

// TODO: Controllers cause some issues on wine, they are detected, but are null
int main() {

	flare::init();

	print::m("Welcome to Flare Engine");

	// NOTE: This is code to test joystick input
	int present = glfwJoystickPresent(GLFW_JOYSTICK_2);
	print::d("Present: %i", present);

	if (present == 1) {
		const char *name = glfwGetJoystickName(GLFW_JOYSTICK_1);

		print::d("Controller name: %s", name);

	}
	// END

	fuse::Entity *player = fuse::createEntity("Player");
	player->addComponent<Camera>();
	// Make sure that the controller is plugged in before adding the control
	if (present == 1) {

		player->addComponent<JoyCamera>();
		player->addComponent<JoyDebugControl>();
	}

#if 0
	for (int i = 0; i < 10; ++i) {

		fuse::Entity *cube = fuse::createEntity("Container");
		cube->addComponent<flare::component::Position>(cubePositions[i]);
		cube->addComponent<flare::component::Rotation>(glm::radians(20.0f * i), glm::vec3(0.5f, 1.0f, 0.0f));
		cube->addComponent<flare::component::Object>("cube");
		cube->addComponent<Spin>();
	}
#elif 1
	fuse::Entity *house = fuse::createEntity("House");
	house->addComponent<flare::component::Position>(glm::vec3(0.0f, 0.0f, 0.0f));
	house->addComponent<flare::component::Object>("house");
#elif 0
	fuse::Entity *rungholt = fuse::createEntity("Rungholt");
	rungholt->addComponent<flare::component::Position>(glm::vec3(0.0f, 0.0f, 0.0f));
	rungholt->addComponent<flare::component::Object>("rungholt");

	flare::render::State::Camera *camera = flare::render::getState()->camera;
	camera->position = glm::vec3(377, 115, -246);

	camera->rotation.y = -16.0f;
	camera->rotation.x = 157.0f;
#endif

	while (flare::isRunning()) {

		flare::update();

		// NOTE: Why is this here? (Probably just testing)
		if (flare::input::keyCheck(GLFW_KEY_X) && player != nullptr) {
			player->kill();
			player = nullptr;
			flare::input::keySet(GLFW_KEY_X, false);

			print::d("Killing player");
		}
	}

	flare::terminate();
}
