#include "StartScene.h"

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
bool StartScene::init(const shared_ptr<AssetManager>& assets) {
    if (!GameMode::init(assets, constants::GameMode::Start, "start")) return false;

    _create = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("start_create"));
    if (_create == nullptr) {
        return false;
    }
    _create->addListener([=](const string& name, bool down) {
        if (!down) {
            _mode = constants::GameMode::CreateGame;
        }
        });

    _join = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("start_invite"));
    if (_join == nullptr) {
        return false;
    }
    _join->addListener([=](const string& name, bool down) {
        if (!down) {
            _mode = constants::GameMode::JoinGame;
        }
        });
    
    _muteButton = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("start_muteButton"));
    if (_muteButton == nullptr) {
        return false;
    }

    _muteButton->addListener([=](const string& name, bool down) {
        if (!down) {
            _mute = !_mute;
            CULog("MUTING BUTTON %d", _mute);
        }
        });

    if (_active) {
        _create->activate();
        _join->activate();
        _muteButton->activate();
    }
    _mode = constants::GameMode::Start;
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void StartScene::dispose() {
    setActive(false);
    _create = nullptr;
    _join = nullptr;
    _muteButton->clearListeners();
    _muteButton = nullptr;
    GameMode::dispose();
}

/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void StartScene::setActive(bool value) {
    _active = value;
    if (value) {
        if (!_create->isActive()) {
            _create->activate();
        }
        if (!_join->isActive()) {
            _join->activate();
        }
        if (!_muteButton->isActive()) {
            _muteButton->activate();
        }
    }
    else {
        if (_create->isActive()) {
            _create->deactivate();
        }
        if (_join->isActive()) {
            _join->deactivate();
        }
        if (_muteButton->isActive()) {
            _muteButton->deactivate();
        }
    }
}
