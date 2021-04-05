#include "PlayerGhost.h"

using namespace cugl;

/**
 * Sets the film strip representing this Ghost.
 *
 * Setting this to nullptr clears the value.
 *
 * @param value The Ghost film strip.
 */
void Ghost::setNode(const std::shared_ptr<scene2::AnimationNode>& value) {
    Player::setNode(value);
    if (_node != nullptr) {
        _node->setFrame(GHOST_IMG_FRONT);
    }
}

/**
 * Updates the state of the model
 *
 * This method moves the Ghost forward, dampens the forces (if necessary)
 * and updates the sprite if it exists.
 *
 * @param timestep  Time elapsed since last called.
 */

void Ghost::update(float timestep) {
    Player::update(timestep);

    if (_move == Vec2::ZERO) {
        _idle = true;
    } else {
        _idle = false;
    }
    
    // Move the ghost
    _loc += _move * _speed;
    
    processDirection();

    if (_node != nullptr) {
        _node->setPosition(_loc);
        _node->setPriority(-_loc.y);
    }
}

void Ghost::processDirection() {
    unsigned int frame = _node->getFrame();
    switch (isDirection()) {
        case Direction::Top:
            if (_idle && frame != GHOST_IMG_BACK) {
                frame = GHOST_IMG_BACK;
            }
            if (!_idle && frame >= GHOST_IMG_BACK && frame < GHOST_IMG_LAST) {
                frame += 1;
                if (frame == GHOST_IMG_LAST) {
                    frame = GHOST_IMG_BACK + 1;
                }
            }
            else if (!_idle) {
                frame = GHOST_IMG_BACK+ 1;
            }
            break;
        case Direction::Bottom:
            if (_idle && frame != GHOST_IMG_FRONT) {
                frame = GHOST_IMG_FRONT;
            }
            if (!_idle && frame >= GHOST_IMG_FRONT && frame < GHOST_IMG_BACK) {
                frame += 1;
                if (frame == GHOST_IMG_BACK) {
                    frame = GHOST_IMG_FRONT + 1;
                }
            }
            else if (!_idle) {
                frame = GHOST_IMG_FRONT + 1;
            }
            break;
        case Direction::Right:
            if (_idle && frame != GHOST_IMG_RIGHT) {
                frame = GHOST_IMG_RIGHT;
            }
            if (!_idle && frame >= GHOST_IMG_RIGHT && frame < GHOST_IMG_LEFT) {
                frame += 1;
                if (frame == GHOST_IMG_LEFT) {
                    frame = GHOST_IMG_RIGHT + 1;
                }
            }
            else if (!_idle) {
                frame = GHOST_IMG_RIGHT + 1;
            }
            break;
        case Direction::Left:
            if (_idle && frame != GHOST_IMG_LEFT) {
                frame = GHOST_IMG_LEFT;
            }
            else if (!_idle && frame >= GHOST_IMG_LEFT && frame < GHOST_IMG_FRONT) {
                frame += 1;
                if (frame == GHOST_IMG_FRONT) {
                    frame = GHOST_IMG_LEFT + 1;
                }
            }
            else if (!_idle) {
                frame = GHOST_IMG_LEFT + 1;
            }
            break;
    }
    _node->setFrame(frame);
}

/**
 * Resets the pal back to its original settings
 */
void Ghost::reset() {
    Player::reset();

    if (_node != nullptr) {
        _node->setFrame(GHOST_IMG_FRONT);
    }
}
