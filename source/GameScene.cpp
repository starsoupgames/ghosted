#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "GameScene.h"
#include "GameEntities/PlayerPal.h"
#include "InputController.h"

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

/** The parallax for each layer */
#define PARALLAX_AMT 0.1f

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
    Rect bounds = Application::get()->getDisplayBounds();
    dimen *= SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::init(dimen)) {
        return false;
    }

    // Start up the input handler
    _assets = assets;
    _input.init(1, bounds);

    auto scene = _assets->get<scene2::SceneNode>("game");
    scene->setContentSize(dimen);
    //scene->doLayout(); //possibly obsolute ship code

    printf("testing \n");
    _palNode = std::dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_field_player_pal_base"));

    // Create the pal model
    Vec2 palPos = _palNode->getPosition();
    _palModel = Pal::alloc(palPos);
    _palModel->setNode(_palNode);

    addChild(scene);
    return true;
}

/**
*Disposes of all(non - static) resources allocated to this mode.
*/
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _input.dispose();
        _palNode = nullptr;
        _palModel = nullptr;
        _active = false;
    }
}

#pragma mark -
#pragma mark Gameplay Handling

/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    // Reset the ships and input
    //_palModel->reset();
    //_input.clear();
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    _input.update(timestep);
    CULog("testinggg");

    //resets the game if necessary
    //if (_input.didReset()) { reset(); }

    Vec2 move = _input.getMove();
    int turning = _input.getTurn();

    // Move the pal (MODEL ONLY)
    _palModel->setForward(move.y);
    _palModel->setSide(move.x);
    _palModel->processTurn(turning);
    _palModel->update(timestep);

}