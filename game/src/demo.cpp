#include "standard.h"

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

struct Spin : fuse::Component {

    flare::component::Rotation *rotation = nullptr;
    GLfloat rotationBase = 0;

    void _update() override {

	static bool toggle = true;
	if (flare::input::keyCheck(GLFW_KEY_F)) {

	    if (toggle) {
		toggle = false;
	    } else {
		toggle = true;
	    }
	    flare::input::keySet(GLFW_KEY_F, false);
	}
	if (toggle) {
	    rotation->rotation = rotationBase + glm::radians(glfwGetTime() * 20.0f);
	}
    }

    Spin(fuse::Entity *parent) {
	super(parent);

	rotation = parent->getComponent<flare::component::Rotation>();
	rotationBase = rotation->rotation;
    }
};

// struct LuaTest : fuse::Component {
//
//     LuaTest(fuse::Entity *parent) {
// 	super(parent);
//
// 	LuaContext lua;
// 	lua.writeVariable("x", 5);
// 	lua.executeCode("x = x + 2;");
// 	print::d("Lua: %i", lua.readVariable<int>("x"));
//
// 	lua.executeCode("print('Hello from Lua!');");
//     }
//
// };

// TODO: Functions like these need unit testing
float nfmod(float a, float b) {
	
	return a - b * floor(a / b);
}

// TODO: Old engine camera should be merged in here, the engine should not come with camera controls
struct JoyCamera : fuse::Component {

	JoyCamera(fuse::Entity *parent) {
		super(parent);
	}

	void _update() override {

		flare::render::Camera *camera = flare::render::getCamera();
		GLfloat speed = 50.0f * flare::render::getDeltaTime();

		// TODO: Create easier controller support in engine
		int countAxes;
		const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &countAxes);

		camera->position -= speed * glm::normalize(glm::vec3(camera->front.x, 0, camera->front.z)) * axes[1];
		camera->position += glm::normalize(glm::cross(camera->front, camera->up)) * speed * axes[0];

		// Using the mouse yaw and pitch so keyboard and controller can be used at the same time
		// TODO: Pitch and yaw should be build into the camera, kinda weird to store it in the mouse
		flare::input::Mouse *mouse = flare::input::getMouse();

		// TODO: Add sensitivity
		mouse->pitch -= 10 * speed * axes[3];
		mouse->yaw += 10 * speed * axes[2];

		mouse->pitch = nfmod(mouse->pitch, 360);
		mouse->yaw = nfmod(mouse->yaw, 360);

		// TODO: Should this be calculated in the camera? (probably not, allows for customization)
		camera->front.x = cos(glm::radians(mouse->yaw)) * cos(glm::radians(mouse->pitch));
		camera->front.y = sin(glm::radians(mouse->pitch));
		camera->front.z = sin(glm::radians(mouse->yaw)) * cos(glm::radians(mouse->pitch));
		camera->front = glm::normalize(camera->front);

		int countButtons;
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &countButtons);

		camera->position += speed * camera->up * (float)buttons[14];
		camera->position -= speed * camera->up * (float)buttons[13];
	}

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
    player->addComponent<flare::component::Camera>();
	// Make sure that the controller is plugged in before adding the control
	if (present == 1) {

		player->addComponent<JoyCamera>();
		player->addComponent<JoyDebugControl>();
	}

    // for (int i = 0; i < 10; ++i) {
    //
	// fuse::Entity *cube = fuse::createEntity("Container");
	// cube->addComponent<flare::component::Position>(cubePositions[i]);
	// cube->addComponent<flare::component::Rotation>(glm::radians(20.0f * i), glm::vec3(0.5f, 1.0f, 0.0f));
	// cube->addComponent<flare::component::Object>("cube", "base/object");
	// cube->addComponent<Spin>();
    // }
	
	fuse::Entity *house = fuse::createEntity("Rungholt");
	house->addComponent<flare::component::Position>(glm::vec3(0.0f, 0.0f, 0.0f));
	house->addComponent<flare::component::Object>("house", "base/object");

    // fuse::Entity *test = fuse::createEntity("Test");
    // test->addComponent<LuaTest>();

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
