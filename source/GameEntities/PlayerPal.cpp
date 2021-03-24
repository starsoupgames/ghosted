// PlayerPalModel.cpp

#include "PlayerPal.h"

using namespace cugl;

#pragma mark -
#pragma mark Animation Constants and Functions

/**
* Returns a newly allocated Pal at the given position
* @param pos Initial position in world coordinates
*
* @return a newly allocated Pal at the given position
*/
bool Pal::init(const Vec2& pos) {
    _id = 1;
    GameEntity::init(pos);
    return true;
}


/**
 * Disposes all resources and assets of this pal
 */
void Pal::dispose() {
    _palNode = nullptr;
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
void Pal::setNode(const std::shared_ptr<scene2::AnimationNode>& value) {
    GameEntity::setNode(value);
    _palNode = GameEntity::getSprite();
    if (_palNode != nullptr) {
        _palNode->setFrame(PAL_IMG_FRONT);
        _idle = true;
        _front = true;
        _back = false;
        _left = false;
        _right = false;
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
    GameEntity::update(timestep);

    determineAction();

    // Move the pal
    _loc += _move * speed;
    // CULog("x: %f, y : %f", _loc.x, _loc.y);
    
    if (_palNode != nullptr) {
        advanceFrame();
        _palNode->setPosition(_loc);
    }
}

void Pal::determineAction() {
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

/**
 * Determines the next animation frame for the pal and applies it to the sprite.
 *
 * This method includes some dampening of the turn, and should be called before
 * moving the pal.
 */
void Pal::advanceFrame() {
    // Our animation depends on the current frame.
    unsigned int frame = _palNode->getFrame();
    if (_idle) {
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

    // Process the pal direction
    //finishing up walking animation after stopping
    /*if (_occupied && _forward == 0 && _side == 0) {
        if (_front && frame < PAL_IMG_BACK) {
            frame += 1;
            if (frame == PAL_IMG_BACK) _occupied = false;
        }
        else if (_back && frame < PAL_IMG_LAST) {
            frame += 1;
            if (frame == PAL_IMG_LAST) _occupied = false;
        }
        else if (_right && frame < PAL_IMG_LEFT) {
            frame += 1;
            if (frame == PAL_IMG_LEFT) _occupied = false;
        }
        else if (_left && frame < PAL_IMG_FRONT) {
            frame += 1;
            if (frame == PAL_IMG_FRONT) _occupied = false;
        }
        if (!_occupied) _idle = true;

    } else if (_idle && (_forward != 0.0f || _side != 0.0f)) {
        _walking = true;
        _occupied = true;
        if (_forward > 0.0f) {
            frame = PAL_IMG_FRONT + 1;
            _front = true;
            _back = false;
            _left = false;
            _right = false;
        }
        else if (_forward < 0.0f) {
            frame = PAL_IMG_BACK + 1;
            _front = false;
            _back = true;
            _left = false;
            _right = false;
            
        }
        else if (_side > 0.0f) {
            frame = PAL_IMG_RIGHT + 1;
            _front = false;
            _back = false;
            _left = false;
            _right = true;
            
        }
        else if (_side < 0.0f) {
            frame = PAL_IMG_LEFT + 1;
            _front = false;
            _back = false;
            _left = true;
            _right = false;
        }
    }
    else if (_walking) {

    }*/

    
    _palNode->setFrame(frame);
}

/**
 * Resets the pal back to its original settings
 */
void Pal::reset() {
    GameEntity::reset();

    if (_palNode != nullptr) {
        _palNode->setFrame(PAL_IMG_FRONT);
        _idle = true;
        _front = true;
        _back = false;
        _left = false;
        _right = false;
    }
}
