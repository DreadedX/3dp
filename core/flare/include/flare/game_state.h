#ifndef GAME_STATE_H
#define GAME_STATE_H

namespace flare {

	class GameState {

		public:

			// Stores all the entity needed for this specific game state
			fuse::Manager *manager = nullptr;

			// Stores all renderpass required for game state
			Array<render::passes::Pass*> renderPasses;

			virtual void update() = 0;
			virtual void draw() = 0;

			virtual ~GameState(){}
	};

	class MainState : public GameState {

		public:

			void update() override {

				manager->update();
			}

			void draw() override {

				render::draw(this);
		}
	};

	class PauseState : public GameState {

		public:

			// Stores pointer to previous game state so that it can render on top of the previous state
			GameState *previousState = nullptr;

			void update() override {

				manager->update();
			}

			void draw() override {

				// previousState->manager->draw();
				render::draw(previousState);
				render::draw(this);
			}
	};
}

#endif

