#include "PlayerGhost.h"

using namespace cugl;


void Ghost::setNode(const std::shared_ptr<scene2::AnimationNode>& node, const std::shared_ptr<scene2::PolygonNode>& shadow) {
    _shadow = shadow;
    _spooking = false;
    if (_shadow != nullptr) {
        _shadow->setPosition(_shadow->getPosition() + constants::GHOST_SHADOW_OFFSET);
        _shadow->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
        node->addChildWithName(shadow, "shadow");
    }
    Player::setNode(node);
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
    if (getSpooking() && _spookingTimer < 48) {
        ++_spookingTimer;
    }
    else if (getSpooking() && _spookingTimer >= 48) {
        setSpooking(false);
    }

    processDirection();

    Player::update(timestep);
}

void Ghost::processDirection() {
    unsigned int frame = _node->getFrame();
    if (_timer >= 2) {
        _timer = 0;
        _node->setFrame(frame);
        return;

    }
    _node->setScale(Vec2(1, 1));

    if (_spooking) {
        if (frame < IMG_SPOOKING_START) {
            frame = IMG_SPOOKING_START;
        }
        else if (frame < IMG_SPOOKING_END) {
            ++frame;
        }
        ++_timer;
        _node->setFrame(frame);
        return;

    }

    switch (isDirection()) {
    case Direction::Top:
        if (frame >= IMG_BACK && frame < IMG_LAST - 1) {
            ++frame;
        }
        else {
            frame = IMG_BACK;
        }

        break;
    case Direction::Bottom:
        if (frame >= IMG_FRONT && frame < IMG_BACK - 1) {
            ++frame;
        }
        else {
            frame = IMG_FRONT;
        }
        break;
    case Direction::Right:
        _node->setScale(Vec2(-1, 1));
        if (frame >= IMG_LEFT && frame < IMG_FRONT - 1) {
            ++frame;
        }
        else {
            frame = IMG_LEFT;
        }

        break;
    case Direction::Left:

        if (frame >= IMG_LEFT && frame < IMG_FRONT - 1) {
            ++frame;
        }
        else {
            frame = IMG_LEFT;
        }

        break;
    }
    CUAssertLog(frame < IMG_LAST, "Frame out of range.");
    ++_timer;
    _node->setFrame(frame);
}

/**
 * Resets the pal back to its original settings
 */
void Ghost::reset() {
    Player::reset();

    if (_node != nullptr) {
        _node->setFrame(IMG_FRONT);
    }
}
