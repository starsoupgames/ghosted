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

    // Move the ghost
    _loc += _move * speed;
    // CULog("x: %f, y : %f", _loc.x, _loc.y);

    if (_node != nullptr) {
        determineAction();
        advanceFrame();
        _node->setPosition(_loc);
        CULog("ghost y %f", _loc.y);
        _node->setPriority(-_loc.y);
        CULog("ghost priority %f", _node->getPriority());
    }
}

/**
 * Determines the next animation frame for the Ghost and applies it to the sprite.
 *
 * This method includes some dampening of the turn, and should be called before
 * moving the Ghost.
 */

void Ghost::determineAction() {
    if (_move == Vec2::ZERO) {
        _idle = true;
        return;
    }
    else {
        _idle = false;
        _front = false;
        _back = false;
        _left = false;
        _right = false;
        if (_move.y < 0) {
            _front = true;
        }
        else if (_move.y > 0) {
            _back = true;
        }
        else if (_move.x < 0) {
            _left = true;
        }
        else if (_move.x > 0) {
            _right = true;
        }
    }
}
void Ghost::advanceFrame() {
    // Our animation depends on the current frame.
    unsigned int frame = _node->getFrame();
    if (_idle) {
        if (_front) {
            frame = GHOST_IMG_FRONT;
        }
        else if (_back) {
            frame = GHOST_IMG_BACK;
        }
        else if (_right) {
            frame = GHOST_IMG_RIGHT;
        }
        else {
            frame = GHOST_IMG_LEFT;
        }
    }
    else {
        if (_front) {
            if (frame >= GHOST_IMG_FRONT && frame < GHOST_IMG_BACK) {
                frame += 1;
                if (frame == GHOST_IMG_BACK) {
                    frame = GHOST_IMG_FRONT + 1;
                }
            }
            else {
                frame = GHOST_IMG_FRONT + 1;
            }
        }
        else if (_back) {
            if (frame >= GHOST_IMG_BACK && frame < GHOST_IMG_LAST) {
                frame += 1;
                if (frame == GHOST_IMG_LAST) {
                    frame = GHOST_IMG_BACK + 1;
                }
            }
            else {
                frame = GHOST_IMG_BACK + 1;
            }
        }
        else if (_right) {
            if (frame >= GHOST_IMG_RIGHT && frame < GHOST_IMG_LEFT) {
                frame += 1;
                if (frame == GHOST_IMG_LEFT) {
                    frame = GHOST_IMG_RIGHT + 1;
                }
            }
            else {
                frame = GHOST_IMG_RIGHT + 1;
            }
        }
        else {
            if (frame >= GHOST_IMG_LEFT && frame < GHOST_IMG_FRONT) {
                frame += 1;
                if (frame == GHOST_IMG_FRONT) {
                    frame = GHOST_IMG_LEFT + 1;
                }
            }
            else {
                frame = GHOST_IMG_LEFT + 1;
            }
        }
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
