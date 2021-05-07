#include "PlayerPal.h"

using namespace cugl;


void Pal::setNode(const std::shared_ptr<scene2::AnimationNode>& node, const std::shared_ptr<scene2::PolygonNode>& shadow, const std::shared_ptr<scene2::AnimationNode>& smoke) {
    _shadow = shadow;
    _smokeNode = smoke;
    if (_shadow != nullptr) {
        _shadow->setPosition(Vec2(node->getWidth() / 2, 0));
        node->addChildWithName(shadow, "shadow");
    }
    if (_smokeNode != nullptr) {
        _smokeNode->setPosition(_smokeNode->getPosition() + constants::PAL_SMOKE_OFFSET);
        _smokeNode->setFrame(0);
        _smokeNode->setVisible(false);
        node->addChildWithName(smoke, "smoke");
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
    if (_spooked) {
        _velocity = Vec2::ZERO;
    }

    processDirection();

    Player::update(timestep);
}

void Pal::processDirection() {
    unsigned int frame = _node->getFrame();
    if (_timer >= 2) {
        _timer = 0;
        _node->setFrame(frame);
        return;

    }
    _node->setScale(Vec2(1, 1));
    if (_spooked) { 
        unsigned int smokeFrame = _smokeNode->getFrame();
        if (frame < IMG_SPOOK_CHANGE) {
            frame = IMG_SPOOK_CHANGE;
        }
        else if (frame < IMG_SPOOKED_END) {
            ++frame;
        }
        else {
            frame = IMG_SPOOKED_START;
        }
        if (frame == IMG_SMOKE_FRAME_START) {
            _smokeNode->setVisible(true);
        }
        else if (frame > IMG_SMOKE_FRAME_START && smokeFrame < IMG_SMOKE_END) {
            ++smokeFrame;
        }
        else if (smokeFrame == IMG_SMOKE_END) {
            smokeFrame = 0;
            _smokeNode->setVisible(false);
        }
        _smokeNode->setFrame(smokeFrame);
        
    }
    else {
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

    CUAssertLog(frame <= IMG_SPOOKED_END, "Frame out of range.");
    ++_timer;
    _node->setFrame(frame);
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
