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
 * @param node the Player animation node
 * @param shadow the Player shadow node
 */
void Player::setNode(const std::shared_ptr<scene2::AnimationNode>& value, const std::shared_ptr<scene2::PolygonNode>& shadow) {
    Player::setNode(value);
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
    _node = GameEntity::getNode();
    if (_node != nullptr) {
        _timer = 0;
        _idle = true;
        _node->setFrame(IMG_FRONT);
        _node->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
    }
}

/**
 * Updates the state of the model
 *
 * @param timestep  Time elapsed since last called.
 */
void Player::update(float timestep) {
    _loc += _velocity;
    GameEntity::update(timestep);
}

/** Returns the current direction ENUM of the Player
 *
 * @return the current direction of the Player
 */
uint8_t Player::isDirection() {
  uint8_t dir = Direction::Bottom;
  float minDist = _direction.distance(Vec2(0, -1));

  if (_direction.distance(Vec2(0, 1)) < minDist) {
    dir = Direction::Top;
    minDist = _direction.distance(Vec2(0, 1));
  }
  if (_direction.distance(Vec2(1, 0)) <= minDist) {
    dir = Direction::Right;
    minDist = _direction.distance(Vec2(1, 0));
  }
  if (_direction.distance(Vec2(-1, 0)) <= minDist) {
    dir = Direction::Left;
  }
  return dir;
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
