#include "BatteryCollectible.h"

using namespace cugl;

/**
* Returns a newly allocated Battery at the given location
* @param loc Initial location in world coordinates
*
* @return a newly allocated Battery at the given location
*/
bool Battery::init(const Vec2& loc) {
    drawable = true;
    deleteFlag = false;
    GameEntity::init(loc, 10);
    return true;
}

/**
 * Disposes all resources and assets of this battery
 */
void Battery::dispose() {
    _batteryTexture = nullptr;
}