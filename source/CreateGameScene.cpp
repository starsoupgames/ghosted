#include "CreateGameScene.h"

using namespace std;
using namespace cugl;

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool CreateGameScene::init(const shared_ptr<AssetManager>& assets) {
    if (!GameMode::init(assets, constants::GameMode::CreateGame)) return false;

    _mode = constants::GameMode::Lobby;

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void CreateGameScene::dispose() {
    GameMode::dispose();
    setActive(false);
}

/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void CreateGameScene::setActive(bool value) {
    _active = value;
}
