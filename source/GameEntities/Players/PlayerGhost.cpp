#include "PlayerGhost.h"

using namespace cugl;

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
    _loc += _move * _speed;
    
    Player::processDirection();

    if (_node != nullptr) {
        _node->setPosition(_loc);
        _node->setPriority(-_loc.y);
    }
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
