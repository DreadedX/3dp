#ifndef DEBUG_H
#define DEBUG_H

#ifndef NDEBUG

namespace flare {

	/** @brief Debug functions */
	namespace debug {

		/** Use imgui to create a browsable entity-component tree */
		void entityTree();
		void general();
		void timers();

		void debug();
	}
}

#define DEBUG_TIME_INIT flare::Timer timer("empty", 0);
#define DEBUG_TIME(FUNC, DESC) timer = Timer(DESC, glfwGetTime()); FUNC; timer.time = glfwGetTime() - timer.time; getState()->timers.push_back(timer)
#else

#define DEBUG_TIME_INIT
#define DEBUG_TIME(FUNC, DESC) FUNC
#endif // NDEBUG

#endif
