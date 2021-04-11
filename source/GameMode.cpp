#include "GameMode.h"

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
bool GameMode::init(const shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= constants::SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution

    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::init(dimen)) {
        return false;
    }

    _assets = assets;
    return true;
}

bool GameMode::init(const std::shared_ptr<AssetManager>& assets, const string node) {
    if (!init(assets)) return false;

    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= constants::SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution

    _root = assets->get<scene2::SceneNode>(node);
    if (_root == nullptr) {
        return false;
    }
    _root->setContentSize(dimen);
    _root->doLayout(); // This rearranges the children to fit the screen
    addChild(_root);

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameMode::dispose() {
    _assets = nullptr;
    Scene2::dispose();
}