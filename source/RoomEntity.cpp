#include "RoomEntity.h"

using namespace cugl;

bool RoomEntity::init(const cugl::Vec2& loc, const int radius) {
    _loc = loc;
    _radius = radius;
    if (_animationNode != nullptr) {
        _animationNode->setPosition(_loc);
    }
    return true;
}

void RoomEntity::dispose() {
    _animationNode = nullptr;
}

void RoomEntity::update(float timestep) {}

void RoomEntity::reset() {}
