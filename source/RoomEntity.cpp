#include "RoomEntity.h"

using namespace cugl;

bool RoomEntity::init(const cugl::Vec2& loc) {
    _loc = loc;
    if (_node != nullptr) {
        _node->setPosition(_loc);
    }
    return true;
}

void RoomEntity::update(float timestep) {}

void RoomEntity::reset() {}
