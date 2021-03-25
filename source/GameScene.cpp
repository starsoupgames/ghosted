#include "GameScene.h"
#include "CollisionController.h"

using namespace cugl;
using namespace std;

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

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

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    Rect bounds = Application::get()->getSafeBounds();
    dimen *= SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }
    if (!Scene2::init(dimen)) {
        return false;
    }

    _mode = 2;

    // Init data models
    _networkData = NetworkData::alloc();
    CUAssertLog(_networkData != nullptr, "NetworkData failed allocation.");

    // Init controllers
    _input.init(bounds);
    _network.init(_networkData);

    _assets = assets;

    _root = _assets->get<scene2::SceneNode>("game");
    _root->setContentSize(dimen);
    _root->doLayout();
    addChild(_root);

    if (_roomID == "") {
        _network.connect();
        _host = true;
    }
    else {
        _network.connect(_roomID);
    }

    _palNode = dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_player_pal"));
    //_palNode->setTexture(_assets->get<Texture>("pal_texture"));
    _palModel = Pal::alloc(_palNode->getPosition());
    _palModel->setNode(_palNode);

    _ghostNode = dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_player_ghost"));
    //_ghostedNode->setTexture(_assets->get<Texture>("ghost_texture"));
    _ghostModel = Ghost::alloc(_ghostNode->getPosition());
    _ghostModel->setNode(_ghostNode);

    if (_host) {
        _networkData->setPlayer(_palModel);
        _networkData->setOtherPlayer(_ghostModel);
    }
    else {
        _networkData->setPlayer(_ghostModel);
        _networkData->setOtherPlayer(_palModel);
    }

    return true;
}

/**
*Disposes of all(non - static) resources allocated to this mode.
*/
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _input.dispose();
        _network.dispose();
        _root = nullptr;
        _palNode = nullptr;
        _palModel = nullptr;
        _ghostNode = nullptr;
        _ghostModel = nullptr;
        _active = false;
    }
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH / dimen.width;
    Vec2 center = Vec2(dimen.width / 2, dimen.height / 2);

    _input.update(timestep);
    _network.update(timestep);

    Vec2 move = _input.getMove();
    int turning = _input.getTurn();

    if (_host) {
        _palModel->move(move);
        _palModel->processTurn(turning);

        _root->setPosition(center - _palModel->getLoc());
    }
    else {
        _ghostModel->move(move);

        _root->setPosition(center - _ghostModel->getLoc());
    }

    _palModel->update(timestep);
    _ghostModel->update(timestep);

    // Check win condition
    if (_host) {
        if (_palModel->getSpooked()) {
            // Ghost wins
        }
        else if (_palModel->getBatteries() == 3) {
            // Pal wins
        }
    }
}