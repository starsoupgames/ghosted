#include "LobbyScene.h"

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
bool LobbyScene::init(const shared_ptr<AssetManager>& assets) {
    GameMode::init(assets, constants::GameMode::Lobby, "lobby");

    _start = dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("lobby_create"));
    if (_start == nullptr) {
        return false;
    }
    _start->addListener([=](const string& name, bool down) {
        if (!down) {
            _mode = constants::GameMode::Game;
            _network->startGame();
        }
        });

    _host = _roomID == "";
    if (_host) {
        _network->connect();
    }
    else {
        _network->connect(_roomID);

        Size dimen = Application::get()->getDisplaySize();
        dimen *= constants::SCENE_WIDTH / dimen.width;

        auto roomIDText = scene2::Label::alloc("Room ID: " + _roomID, _assets->get<Font>("gyparody"));
        roomIDText->setForeground(Color4::WHITE);
        roomIDText->setAnchor(Vec2::ANCHOR_TOP_CENTER);
        roomIDText->setPosition(dimen.width / 2, dimen.height);
        _root->addChildWithName(roomIDText, "roomIDText");
    }

    return true;
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void LobbyScene::update(float timestep) {
    Size dimen = Application::get()->getDisplaySize();
    dimen *= constants::SCENE_WIDTH / dimen.width;

    _start->setVisible(_start->isActive());

    if (_network->isConnected()) {
        if (_active && !_start->isActive()) {
            _start->activate();
        }

        if (_roomID != _network->getRoomID() && _network->getRoomID() != "") {
            _roomID = _network->getRoomID();

            auto roomIDText = scene2::Label::alloc("Room ID: " + _roomID, _assets->get<Font>("gyparody"));
            roomIDText->setForeground(Color4::WHITE);
            roomIDText->setAnchor(Vec2::ANCHOR_TOP_CENTER);
            roomIDText->setPosition(dimen.width / 2, dimen.height);
            _root->addChildWithName(roomIDText, "roomIDText");
        };

        if (_numPlayers != _network->getNumPlayers()) {
            _numPlayers = _network->getNumPlayers();

            _root->removeChildByName("numPlayers");

            auto numPlayersText = scene2::Label::alloc("Num players: " + to_string(_numPlayers), _assets->get<Font>("gyparody"));
            numPlayersText->setForeground(Color4::WHITE);
            numPlayersText->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
            numPlayersText->setPosition(dimen.width / 2, dimen.height / 2);
            _root->addChildWithName(numPlayersText, "numPlayers");
        }
    }

    // TODO join room failed, retry connection
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LobbyScene::dispose() {
    GameMode::dispose();
    _network = nullptr;
}

/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void LobbyScene::setActive(bool value) {
    _active = value;
    if (value) {
    }
    else {
        if (_start->isActive()) {
            _start->deactivate();
        }
    }
}