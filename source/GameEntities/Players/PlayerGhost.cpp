#include "PlayerGhost.h"

using namespace cugl;


void Ghost::setNode(const std::shared_ptr<scene2::AnimationNode>& node, const std::shared_ptr<scene2::PolygonNode>& shadow) {
    _shadow = shadow;
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
    // Move the ghost
    _loc += _move * _speed;
    
    processDirection();

    Player::update(timestep);
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
