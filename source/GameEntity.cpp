// PlayerPalModel.cpp

#include "GameEntity.h"

using namespace cugl;

/**
* Returns a newly allocated GameEntity at the given position with given radius
* 
* @param pos    Initial position in physics coordinates
* @param radius The radius of the entity
*
* @return a newly allocated GameEntity at the given position with given radius
*/
bool GameEntity::init(const Vec2& pos) {
    _loc = pos;
    
    return true;
}


/**
 * Disposes all resources and assets of this entity
 */
void GameEntity::dispose() {
    _node = nullptr;
}

/**
 * Sets the film strip representing this entity.
 *
 * Setting this to nullptr clears the value.
 *
 * @param value The pal film strip.
 */
void GameEntity::setNode(const std::shared_ptr<scene2::AnimationNode>& value) {
    _node = value;
    if (_node != nullptr) {
        _node->setPosition(_loc);
        _node->setAnchor(Vec2::ANCHOR_CENTER);
    }
}


/**
 * Updates the state of the model
 *
 * @param timestep  Time elapsed since last called.
 */

void GameEntity::update(float timestep) {
    //setPosition(0, 0);
    if (_node != nullptr) _node->setPosition(_loc);

}

/**
 * Resets the pal back to its original settings
 */
void GameEntity::reset() {
}
