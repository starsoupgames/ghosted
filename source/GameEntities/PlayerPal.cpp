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
    _palTexture = nullptr;
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
    _palTexture = GameEntity::getSprite();
    if (_palTexture != nullptr) {
        _palTexture->setFrame(PAL_IMG_FRONT);
        _idle = true;
        _front = true;
        _back = false;
        _left = false;
        _right = false;
        //_palTexture->setPosition(_position);
        //_palTexture->setAnchor(Vec2::ANCHOR_CENTER);
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
    determineAction();

    if (_palTexture != nullptr) {
        advanceFrame();
    }


    // Move the pal
    Vec2 pos = GameEntity::getLoc() + _move;
    CULog("x: %f y : %f", pos.x, pos.y);
    GameEntity::setLoc(pos);
}

void Pal::determineAction() {
    CULog("forward %f side %f ", _forward, _side);
    //nothing changes
    if (_move.y == _forward && _move.x == _side) {
        _idle = true;
        return;
    }

    if (_forward == 0.0f && _side == 0.0f) {
        _move.y = _forward;
        _move.x = _side;
        _idle = true;
        return;
    }
    else {
        _idle = false;
        _front = false;
        _back = false;
        _left = false;
        _right = false;
        if (_forward < _move.y) {
            _front = true;
        }
        else if (_forward > _move.y) {
            _back = true;
        }
        else if (_side > _move.x) {
            _right = true;
        }
        else {
            _left = true;
        }
        _move.y = _forward;
        _move.x = _side;
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
    unsigned int frame = _palTexture->getFrame();
    if (_idle) {
        CULog("idle");
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
        CULog("walking");
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

    
    _palTexture->setFrame(frame);
}

/**
 * Resets the pal back to its original settings
 */
void Pal::reset() {
    GameEntity::reset();

    if (_palTexture != nullptr) {
        _palTexture->setFrame(PAL_IMG_FRONT);
        _idle = true;
        _front = true;
        _back = false;
        _left = false;
        _right = false;
    }
}
