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

    _mode = 2;

    // Init controllers
    _input.init(1, bounds);
    _network.init();
    if (_roomID == "") {
        _network.connect();
    }
    else {
        _network.connect(_roomID);
    }

    _assets = assets;
    auto scene = _assets->get<scene2::SceneNode>("game");
    scene->setContentSize(dimen);
    scene->doLayout();
    addChild(scene);

    // Create the pal model
    _palNode = dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_field_player_pal_base"));
    Vec2 palPos = _palNode->getPosition();
    _palModel = Pal::alloc(palPos);
    _palModel->setNode(_palNode);

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
    _input.update(timestep);
    _network.update(timestep);

    Vec2 move = _input.getMove();
    int turning = _input.getTurn();

    // Move the pal (MODEL ONLY)
    _palModel->setForward(move.y);
    _palModel->setSide(move.x);
    _palModel->processTurn(turning);
    // _palModel->update(timestep);

}