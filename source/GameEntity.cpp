// PlayerPalModel.cpp

#include "GameEntity.h"

using namespace cugl;

#pragma mark -
#pragma mark Animation Constants and Functions



/**
* Returns a newly allocated Pal at the given position
* @param pos Initial position in world coordinates
*
* @return a newly allocated Pal at the given position
*/
bool GameEntity::init(const Vec2& pos) {
    _initial = pos;
    _loc = pos;
    return true;
}

/**
 * Disposes all resources and assets of this pal
 */
void GameEntity::dispose() {
    _animationNode = nullptr;
}

#pragma mark -
#pragma mark Animation
/**
 * Sets the film strip representing this pal.
 *
 * Setting this to nullptr clears the value.
 *
 * @param value The pal film strip.
 */
void GameEntity::setNode(const std::shared_ptr<scene2::AnimationNode>& value) {
    _animationNode = value;
    if (_animationNode != nullptr) {
        _animationNode->setPosition(_loc);
        _animationNode->setAnchor(Vec2::ANCHOR_CENTER);
    }
}

/**
 * Updates the state of the model
 *
 * This method moves the pal forward, dampens the forces (if necessary)
 * and updates the sprite if it exists.
 *
 * @param timestep  Time elapsed since last called.
 */

void GameEntity::update(float timestep) {
    if (_animationNode != nullptr) {
        advanceFrame();
    }
}

/**
 * Determines the next animation frame for the pal and applies it to the sprite.
 *
 * This method includes some dampening of the turn, and should be called before
 * moving the pal.
 */
void GameEntity::advanceFrame() {
    // Our animation depends on the current frame.
    unsigned int frame = _animationNode->getFrame();
    frame++;
    _animationNode->setFrame(frame);
}

/**
 * Resets the pal back to its original settings
 */
void GameEntity::reset() {
    _loc = _initial;
}
