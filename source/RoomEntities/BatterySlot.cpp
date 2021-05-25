#include "BatterySlot.h"

using namespace cugl;

void BatterySlot::setNode(const shared_ptr<scene2::PolygonNode>& nodeOn, const shared_ptr<scene2::PolygonNode>& nodeOff) {
    _nodeOn = nodeOn;
    _nodeOff = nodeOff;
}

void BatterySlot::update(float timestep) {
    RoomEntity::update(timestep);
    if (_charge > 0) {
        _nodeOn->setVisible(true);
        _nodeOff->setVisible(false);
        _charge -= timestep;
        _activate = false;
    }
    else {
        _nodeOff->setVisible(true);
        _nodeOn->setVisible(false);
    }
}

void BatterySlot::dispose() {
    RoomEntity::dispose();
    _nodeOn = nullptr;
    _nodeOff = nullptr;
}

void BatterySlot::reset() {
    RoomEntity::reset();
    _charge = 0;
}
