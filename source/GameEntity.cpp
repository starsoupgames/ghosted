// PlayerPalModel.cpp

#include "GameEntity.h"

using namespace cugl;

/**
* Returns a newly allocated GameEntity at the given position with given radius
* 
* @param pos Initial position in world coordinates
* @param radius The radius of the entity
*
* @return a newly allocated GameEntity at the given position with given radius
*/
bool GameEntity::init(const Vec2& pos, const int radius) {
    _radius = radius;
    _loc = pos;
    return true;
}

/**
 * Disposes all resources and assets of this entity
 */
void GameEntity::dispose() {
    _animationNode = nullptr;
}

/**
 * Sets the film strip representing this entity.
 *
 * Setting this to nullptr clears the value.
 *
 * @param value The pal film strip.
 */
void GameEntity::setNode(const std::shared_ptr<scene2::AnimationNode>& value) {
    _animationNode = value;
    if (_animationNode != nullptr) {
        _animationNode->setPosition(_loc);
        _animationNode->setAnchor(Vec2::ANCHOR_CENTER);
    }
}

/**
 * Updates the state of the model
 *
 * @param timestep  Time elapsed since last called.
 */

void GameEntity::update(float timestep) {
}

/**
 * Resets the pal back to its original settings
 */
void GameEntity::reset() {
}
