#include "BatterySlot.h"

using namespace cugl;

void BatterySlot::setNode(const shared_ptr<scene2::PolygonNode>& node) {
    _node = node;
}

void BatterySlot::update(float timestep) {
    RoomEntity::update(timestep);
    _charge -= timestep;
//    if (_charge <= 0) {
//        _node->setColor(Color4f::RED);
//    }
}

void BatterySlot::dispose() {
    RoomEntity::dispose();
}

void BatterySlot::reset() {
    RoomEntity::reset();
}
