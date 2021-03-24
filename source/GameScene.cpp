#include "GameScene.h"

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

/** Pal Frame Sprite numbers */
//TODO: REPLACE WITH APPROPRIATE PAL FRAME VALUES
#define PAL_IMG_LEFT   0   // Left bank frame
#define PAL_IMG_FLAT   9   // Neutral frame
#define PAL_IMG_RIGHT 17   // Right bank frame

#pragma mark -
#pragma mark Constructors

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

    Vec2 center = Vec2(dimen.width / 2, dimen.height / 2);

    _mode = 2;

    // Init controllers
    _input.init(bounds);
    _network.init();

    _assets = assets;

    _root = _assets->get<scene2::SceneNode>("game");
    _root->setContentSize(dimen);
    _root->doLayout();
    addChild(_root);

    if (_roomID == "") {
        _network.connect();

        _palNode = dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_player_pal"));
        //_palNode->setTexture(_assets->get<Texture>("pal_base"));
        _palModel = Pal::alloc(_palNode->getPosition());
        _palModel->setNode(_palNode);
    }
    else {
        _network.connect(_roomID);

        _palNode = dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_player_pal"));
        //_palNode->setTexture(_assets->get<Texture>("pal_base"));
        _palModel = Pal::alloc(_palNode->getPosition());
        _palModel->setNode(_palNode);
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
        _active = false;
    }
}

#pragma mark -
#pragma mark Gameplay Handling

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

    if (_palModel != nullptr) {
        _palModel->move(move);
        _palModel->processTurn(turning);
        _palModel->update(timestep);

        _root->setPosition(center - _palModel->getLoc());
    }
}