#include "Trap.h"

using namespace cugl;

void Trap::setTextures(const std::shared_ptr<Texture>& trap, Size size) {
    _animationNode =  scene2::AnimationNode::alloc(trap, 1, 1, 1);
    _trapTexture = trap;
}

void Trap::update(float timestep) {
    RoomEntity::update(timestep);
}

void Trap::dispose() {
    RoomEntity::dispose();
    _trapTexture = nullptr;
}

void Trap::reset() {
    RoomEntity::reset();

    if (_animationNode != nullptr) {
        _animationNode->setFrame(0);
    }
}
