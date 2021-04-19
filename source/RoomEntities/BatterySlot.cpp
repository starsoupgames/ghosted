#include "BatterySlot.h"

using namespace cugl;

void BatterySlot::setTextures(const std::shared_ptr<Texture>& slot, Size size) {
    _animationNode =  scene2::AnimationNode::alloc(slot, 1, 1, 1);
    _slotTexture = slot;
}

void BatterySlot::update(float timestep) {
    RoomEntity::update(timestep);
    _charge -= timestep;
//    if (_charge <= 0) {
//        _animationNode->setColor(Color4f::RED);
//    }
}

void BatterySlot::dispose() {
    RoomEntity::dispose();
    _slotTexture = nullptr;
}

void BatterySlot::reset() {
    RoomEntity::reset();

    if (_animationNode != nullptr) {
        _animationNode->setFrame(0);
    }
}
