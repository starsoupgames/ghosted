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
    _node = GameEntity::getSprite();
    if (_node != nullptr) {
        _timer = 0;
        _idle = true;
        _node->setFrame(IMG_FRONT);
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

/** Processes the direction for the animation and vision cone */
void Player::processDirection() {
    unsigned int frame = _node->getFrame();
    if (_timer >= 2) {
        _timer = 0;
        _node->setFrame(frame);
        return;

    }
    _node->setScale(Vec2(1, 1));
    switch (isDirection()) {
    case Direction::Top:
        if (_idle) {
            frame = IMG_BACK;
        }
        else {
            if (frame >= IMG_BACK && frame < IMG_LAST - 1) {
                ++frame;
            }
            else {
                frame = IMG_BACK + 1;
            }
        }
        break;
    case Direction::Bottom:
        if (_idle) {
            frame = IMG_FRONT;
        }
        else {
            if (frame >= IMG_FRONT && frame < IMG_BACK - 1) {
                ++frame;
            }
            else {
                frame = IMG_FRONT + 1;
            }
        }
        break;
    case Direction::Right:
        _node->setScale(Vec2(-1, 1));
        
        if (_idle) {
            frame = IMG_LEFT;
        }
        else {
            if (frame >= IMG_LEFT && frame < IMG_FRONT - 1) {
                ++frame;
            }
            else {
                frame = IMG_LEFT + 1;
            }
        }
        break;
    case Direction::Left:
        
        if (_idle) {
            frame = IMG_LEFT;
        }
        else {
            if (frame >= IMG_LEFT && frame < IMG_FRONT - 1) {
                ++frame;
            }
            else {
                frame = IMG_LEFT + 1;
            }
        }
        break;
    }
    CUAssertLog(frame < IMG_LAST, "Frame out of range.");
    ++_timer;
    _node->setFrame(frame);
};

/**
 * Resets the pal back to its original settings
 */
void Player::reset() {
    GameEntity::reset();

    if (_node != nullptr) {
        _idle = true;
    }
}
