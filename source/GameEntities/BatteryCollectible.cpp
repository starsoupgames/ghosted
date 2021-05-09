#include "BatteryCollectible.h"

using namespace cugl;

void Battery::setNode(const shared_ptr<scene2::PolygonNode>& node) {
    _node = node;

}


/**
* Returns a newly allocated Battery at the given location
* @param loc Initial location in world coordinates
*
* @return a newly allocated Battery at the given location
*/
bool Battery::init(const Vec2& loc) {
    drawable = true;
    deleteFlag = false;
    // TODO: Specify an actual polygon for battery hitbox
    GameEntity::init(loc);
    return true;
}


/**
 * Called when a battery on the floor collide
 */
void Battery::pickUp() {
    _node->setVisible(false);
    destroy();
}