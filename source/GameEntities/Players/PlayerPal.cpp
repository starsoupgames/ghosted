#include "PlayerPal.h"

using namespace cugl;



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
