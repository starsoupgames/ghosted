#include "PlayerPal.h"

using namespace cugl;


void Pal::setNode(const std::shared_ptr<scene2::AnimationNode>& value, const std::shared_ptr<scene2::PolygonNode>& shadow) {
    _shadow = shadow;
    if (_shadow != nullptr) {
        _shadow->setPosition(_shadow->getPosition() + constants::PAL_SHADOW_OFFSET);
        _shadow->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
    }
    Player::setNode(value);
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
    if (!_spooked) {
        _loc += _move * _speed;
    }

    processDirection();
    if (_node != nullptr) {
        _node->setPosition(_loc);
    }
}

void Pal::processDirection() {
    Player::processDirection();
    
    // VISION CONE LOGIC
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
