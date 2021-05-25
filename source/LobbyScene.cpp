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
    
    _start = dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("lobby_startgame"));
    if (_start == nullptr) {
        return false;
    }
    _start->addListener([=](const string& name, bool down) {
        if (!down) {
            _mode = constants::GameMode::Game;
        }
        });
    
    _escape = dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("lobby_cancelgame"));
    if (_escape == nullptr) {
        return false;
    }
    _escape->addListener([=](const string& name, bool down) {
        if (!down) {
            _mode = constants::GameMode::Start;
        }
        });

    _numPlayers = 0;

    _gameMap = GameMap::alloc(_assets);

    _host = _roomID == "";
    if (_host) {
        _network->connect();
        _gameMap->generateRandomMap();
    }
    else {
        _network->connect(_roomID);
        _gameMap->generateRandomMap();

        Size dimen = Application::get()->getDisplaySize();
        dimen *= constants::SCENE_WIDTH / dimen.width;

        auto roomIDText = scene2::Label::alloc("Invite Code: " + _roomID, _assets->get<Font>("script"));
        roomIDText->setForeground(Color4::WHITE);
        roomIDText->setAnchor(Vec2::ANCHOR_TOP_CENTER);
        roomIDText->setPosition(dimen.width / 2, (dimen.height * 0.9));
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
    _escape->setVisible(_escape->isActive());

    if (_network->isConnected()) {
        if (_host && _active && !_start->isActive()) {
            _start->activate();
            _escape->activate();
        }

        if (_roomID != _network->getRoomID() && _network->getRoomID() != "") {
            _roomID = _network->getRoomID();
            
            auto roomIDText = scene2::Label::alloc("Invite Code: " + _roomID, _assets->get<Font>("script"));
            roomIDText->setForeground(Color4::WHITE);
            roomIDText->setAnchor(Vec2::ANCHOR_TOP_CENTER);
            roomIDText->setPosition(dimen.width / 2, dimen.height * 0.9);
            _root->addChildWithName(roomIDText, "roomIDText");
        };

        if (_numPlayers != _network->getNumPlayers()) {
            _numPlayers = _network->getNumPlayers();

            _root->removeChildByName("numPlayers");

            auto numPlayersText = scene2::Label::alloc("Num players: " + to_string(_numPlayers), _assets->get<Font>("script"));
            numPlayersText->setForeground(Color4::WHITE);
            numPlayersText->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
            numPlayersText->setPosition(dimen.width / 2, dimen.height / 2);
            _root->addChildWithName(numPlayersText, "numPlayers");
        }
    }

    if (_network != nullptr) {
        auto networkData = _network->getData();
        if (networkData != nullptr) {
            if (_network->isConnected() && networkData->getStatus() == constants::MatchStatus::InProgress) {
                _mode = constants::GameMode::Game;
            }
            networkData->setGameMap(_gameMap);
        }

        switch (_network->getStatus()) {
        case CUNetworkConnection::NetStatus::Pending:
            break;
        case CUNetworkConnection::NetStatus::Connected:
            break;
        case CUNetworkConnection::NetStatus::Disconnected:
            CULog("Disconnected");
        case CUNetworkConnection::NetStatus::Reconnecting:
            CULog("Reconnecting");
        case CUNetworkConnection::NetStatus::RoomNotFound:
            CULog("RoomNotFound");
        case CUNetworkConnection::NetStatus::ApiMismatch:
            CULog("ApiMismatch");
        case CUNetworkConnection::NetStatus::GenericError:
            CULog("GenericError");
        default:
            _mode = _network->isHost() ? constants::GameMode::Start : constants::GameMode::JoinGame;
            _network->disconnect();
        }
    }
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LobbyScene::dispose() {
    setActive(false);
    _gameMap = nullptr;

    _network = nullptr;
    _start = nullptr;
    _escape = nullptr;

    _root->removeChildByName("roomIDText");
    _root->removeChildByName("numPlayers");
    GameMode::dispose();
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
        if (_escape->isActive()) {
            _escape->deactivate();
        }
    }
}
