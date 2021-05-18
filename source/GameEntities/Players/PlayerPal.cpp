#include "PlayerPal.h"

using namespace cugl;


void Pal::setNode(const std::shared_ptr<scene2::AnimationNode>& node, const std::shared_ptr<scene2::PolygonNode>& shadow, const std::shared_ptr<scene2::AnimationNode>& effect) {
    _spooked = false;
    _helping = 0;
    _unspooking = 0;
    _shadow = shadow;
    _effectNode = effect;
    if (_shadow != nullptr) {
        _shadow->setPosition(Vec2(node->getWidth() / 2, 0));
        node->addChildWithName(shadow, "shadow");
    }
    if (_effectNode != nullptr) {
        _effectNode->setPosition(_effectNode->getPosition() + constants::PAL_EFFECT_OFFSET);
        _effectNode->setFrame(0);
        _effectNode->setVisible(false);
        node->addChildWithName(effect, "effect");
    }
    Player::setNode(node);
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
    // Move the pal
    if (_spooked || getHelping() || getUnspooking()) {
        _velocity = Vec2::ZERO;
    }

    if (!_spooked) {
        processHelping();
    }
    
    processDirection();
    Player::update(timestep);
}

void Pal::processDirection() {
    unsigned int frame = _node->getFrame();
    unsigned int effectFrame = _effectNode->getFrame();
    if (_timer >= 2) {
        _timer = 0;
        _node->setFrame(frame);
        return;

    }
    
    if (_spooked && !getUnspooking()) {
        _node->setScale(Vec2(1, 1));
        //pal frames
        if (frame < IMG_SPOOK_CHANGE) {
            frame = IMG_SPOOK_CHANGE;
        }
        else if (frame < IMG_SPOOKED_END) {
            ++frame;
        }
        else {
            frame = IMG_SPOOKED_START;
        }

        //smoke frames
        if (frame == IMG_SMOKE_FRAME_START) {
            effectFrame = IMG_SMOKE_START;
            _effectNode->setVisible(true);
        }
        else if (frame >= IMG_SMOKE_FRAME_START && effectFrame < IMG_SMOKE_END) {
            ++effectFrame;
        }
        else if (effectFrame == IMG_SMOKE_END) {
            effectFrame = 0;
            _effectNode->setVisible(false);
        }
        _effectNode->setFrame(effectFrame);
        
    }
    else if (getUnspooking()) {
        //pal frames
        if (frame < IMG_UNSPOOK_START || frame > IMG_UNSPOOK_END) {
            frame = IMG_UNSPOOK_START;
        }
        else if (frame >= IMG_UNSPOOK_START && frame < IMG_UNSPOOK_END) {
            ++frame;
        }

        //heart frames
        if (frame == IMG_UNSPOOK_START) {
            effectFrame = IMG_HEART_START;
            _effectNode->setVisible(true);
        }
        else if (frame >= IMG_UNSPOOK_START && effectFrame < IMG_HEART_END) {
            ++effectFrame;
        }
        else {
            effectFrame = 0;
            _effectNode->setVisible(false);
        }
        _effectNode->setFrame(effectFrame);
    }
    else if (getHelping()) {
        _effectNode->setVisible(false);
        if (frame < IMG_HELP_START) {
            frame = IMG_HELP_START;
        }
        else if (frame < IMG_HELP_END) {
            ++frame;
        }
    }
    else {
        _node->setScale(Vec2(1, 1));
        _effectNode->setVisible(false);
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
    }

    CUAssertLog(frame <= IMG_HELP_END, "Frame out of range.");
    ++_timer;
    _node->setFrame(frame);
}

/** Processes the timing for a pal helping */
void Pal::processHelping() {
    if (getHelping()) {
        --_helping;
    }
    else if (getUnspooking()) {
        --_unspooking;
    }
}

/**
 * Resets the pal back to its original settings
 */
void Pal::reset() {
    Player::reset();

    if (_node != nullptr) {
        _node->setFrame(IMG_FRONT);
    }
}
