#include "JoinGameScene.h"

using namespace std;
using namespace cugl;

// https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
bool is_numeric(const string& s) {
    return !s.empty() && s.find_first_not_of("0123456789") == string::npos;
}

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
bool JoinGameScene::init(const shared_ptr<AssetManager>& assets) {
    GameMode::init(assets, "join");

    _field = dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("join_textfield"));
    if (_field == nullptr) {
        return false;
    }
    _field->addExitListener([=](const string& name, const string& value) {
        if (is_numeric(value)) {
            _roomID = value;
            _mode = constants::GameMode::Lobby;
        }
        else {
            CULog("%s is not numeric.", value.c_str());
        }
        });

    if (_active) {
        _field->activate();
    }

    return true;
}

/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void JoinGameScene::setActive(bool value) {
    _active = value;
    if (value && !_field->isActive()) {
        _field->activate();
    }
    else if (!value && _field->isActive()) {
        _field->deactivate();
    }
}