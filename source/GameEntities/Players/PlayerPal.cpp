#include "PlayerPal.h"

using namespace cugl;

/**
 * Sets the film strip representing this pal.
 *
 * Setting this to nullptr clears the value.
 *
 * @param value The pal film strip.
 */
void Pal::setNode(const std::shared_ptr<scene2::AnimationNode>& value) {
    Player::setNode(value);
    if (_node != nullptr) {
        _node->setFrame(PAL_IMG_FRONT);
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

void Pal::update(float timestep) {
    Player::update(timestep);

    // Move the pal
    _loc += _move * speed;
    // CULog("x: %f, y : %f", _loc.x, _loc.y);
    
    if (_node != nullptr) {
        determineAction();
        advanceFrame();
        _node->setPosition(_loc);
        _node->setPriority(-_loc.y);
    }
}

/**
 * Determines the next animation frame for the pal and applies it to the sprite.
 *
 * This method includes some dampening of the turn, and should be called before
 * moving the pal.
 */
void Pal::determineAction() {
    if (_move == Vec2::ZERO) {
        _idle = true;
        return;
    }
    else {
        _idle = false;
        if (_turn.y < 0) {
            _front = true;
            _back = false;
            _left = false;
            _right = false;
            _facing = "front";
        }
        else if (_turn.y > 0) {
            _front = false;
            _back = true;
            _left = false;
            _right = false;
            _facing = "back";
        }
        else if (_turn.x < 0) {
            _front = false;
            _back = false;
            _left = true;
            _right = false;
            _facing = "left";
        }
        else if (_turn.x > 0) {
            _front = false;
            _back = false;
            _left = false;
            _right = true;
            _facing = "right";
        }
    }
}

void Pal::determineDirection() {
    if (_turn.y < 0) {
        _front = true;
        _back = false;
        _left = false;
        _right = false;
        _facing = "front";
    }
    else if (_turn.y > 0) {
        _front = false;
        _back = true;
        _left = false;
        _right = false;
        _facing = "back";
    }
    else if (_turn.x < 0) {
        _front = false;
        _back = false;
        _left = true;
        _right = false;
        _facing = "left";
    }
    else if (_turn.x > 0) {
        _front = false;
        _back = false;
        _left = false;
        _right = true;
        _facing = "right";
    }
}

void Pal::advanceFrame() {
    // Our animation depends on the current frame.
    unsigned int frame = _node->getFrame();
    if (_idle) {
        determineDirection();
        if (_front) {
            frame = PAL_IMG_FRONT;
        }
        else if (_back) {
            frame = PAL_IMG_BACK;
        }
        else if (_right) {
            frame = PAL_IMG_RIGHT;
        }
        else {
            frame = PAL_IMG_LEFT;
        }
    }
    else {
        if (_front) {
            if (frame >= PAL_IMG_FRONT && frame < PAL_IMG_BACK) {
                frame += 1;
                if (frame == PAL_IMG_BACK) {
                    frame = PAL_IMG_FRONT + 1;
                }
            }
            else {
                frame = PAL_IMG_FRONT + 1;
            }
        }
        else if (_back) {
            if (frame >= PAL_IMG_BACK && frame < PAL_IMG_LAST) {
                frame += 1;
                if (frame == PAL_IMG_LAST) {
                    frame = PAL_IMG_BACK + 1;
                }
            }
            else {
                frame = PAL_IMG_BACK+ 1;
            }
        }
        else if (_right) {
            if (frame >= PAL_IMG_RIGHT && frame < PAL_IMG_LEFT) {
                frame += 1;
                if (frame == PAL_IMG_LEFT) {
                    frame = PAL_IMG_RIGHT + 1;
                }
            }
            else {
                frame = PAL_IMG_RIGHT + 1;
            }
        }
        else {
            if (frame >= PAL_IMG_LEFT && frame < PAL_IMG_FRONT) {
                frame += 1;
                if (frame == PAL_IMG_FRONT) {
                    frame = PAL_IMG_LEFT + 1;
                }
            }
            else {
                frame = PAL_IMG_LEFT + 1;
            }
        }
    }
    
    _node->setFrame(frame);
}

/**
 * Resets the pal back to its original settings
 */
void Pal::reset() {
    Player::reset();

    if (_node != nullptr) {
        _node->setFrame(PAL_IMG_FRONT);
    }
}
