#include "Player.h"

using namespace cugl;

/**
* Returns a newly allocated Player at the given position
* @param pos Initial position in world coordinates
*
* @return a newly allocated Player at the given position
*/
bool Player::init(const Vec2& pos) {
    return GameEntity::init(pos);
}

/**
 * Disposes all resources and assets of this Player
 */
void Player::dispose() {
    _node = nullptr;
    _shadow = nullptr;
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

void Player::setHitbox(const std::shared_ptr<scene2::PolygonNode>& value) {
    _hitbox = value;
    if (_node != nullptr && _hitbox != nullptr && _shadow != nullptr) {
        _hitbox->setPosition(Vec2(_node->getWidth() / 2, 0));
        _hitbox->setPriority(_node->getPriority() + 100);
        _node->addChildWithName(_hitbox, "hitbox");
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

Vec2 Player::predictVelocity(Vec2 move) {
    // subtract drag
    Vec2 velocity = _velocity;
    if (velocity.length() <= DRAG) {
        velocity = Vec2::ZERO;
    }
    else {
        velocity -= velocity.normalize() * DRAG;
    }

    if (!move.isNearZero()) {
        // add acceleration
        velocity += move.normalize() * ACCELERATION;

        // clamp to speed
        if (velocity.length() > _speed) {
            velocity = velocity.normalize() * _speed; // clamp to speed
        }
    }

    return velocity;
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
