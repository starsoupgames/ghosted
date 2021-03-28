#include "Player.h"

using namespace cugl;

/**
* Returns a newly allocated Player at the given position
* @param pos Initial position in world coordinates
*
* @return a newly allocated Player at the given position
*/
bool Player::init(const Vec2& pos) {
    return GameEntity::init(pos, 25);
}

/**
 * Disposes all resources and assets of this Player
 */
void Player::dispose() {
    _node = nullptr;
}

/**
 * Sets the animation node representing this Player.
 *
 * Setting this to nullptr clears the value.
 *
 * @param value the Player animation node.
 */
void Player::setNode(const std::shared_ptr<scene2::AnimationNode>& value) {
    GameEntity::setNode(value);
    _node = GameEntity::getSprite();
    if (_node != nullptr) {
        _idle = true;
    }
}

/**
 * Updates the state of the model
 *
 * @param timestep  Time elapsed since last called.
 */

void Player::update(float timestep) {
    GameEntity::update(timestep);
}

/**
 * Resets the pal back to its original settings
 */
void Player::reset() {
    GameEntity::reset();

    if (_node != nullptr) {
        _idle = true;
    }
}


