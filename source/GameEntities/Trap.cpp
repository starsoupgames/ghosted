#include "Trap.h"

using namespace cugl;

void Trap::update(float timestep) {
    GameEntity::update(timestep);
    if (getTriggered()) {
        if (_triggering > 0) {
            _triggering--;
        }
        if (doneTriggering()) {
            _animationNode->setVisible(false);
        }
        else if (!justTriggered()) {
            _animationNode->getChildByName("radius")->setVisible(false);
        }
    }
    else if (getArmed()) {
        if (_arming > 0) {
            _arming--;
        }
        else if (doneArming()) {
            _animationNode->getChildByName("radius")->setVisible(true);
        }
    }
}

void Trap::dispose() {
    GameEntity::dispose();
}

void Trap::reset() {
    GameEntity::reset();

    if (_animationNode != nullptr) {
        _animationNode->setFrame(0);
    }
}
