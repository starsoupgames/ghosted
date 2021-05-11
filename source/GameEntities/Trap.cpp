#include "Trap.h"

using namespace cugl;

void Trap::setNode(const shared_ptr<scene2::PolygonNode>& node, const std::shared_ptr<scene2::AnimationNode>& chandelier) {
    _node = node;
    _chandelierNode = chandelier;
    if (_chandelierNode != nullptr) {
        //_chandelierNode->setPosition(node->getNodeToWorldTransform().transformVector((Vec2(node->getWidth(), node->getHeight()) * 2)));
        _timer = 0;
        _chandelierNode->setFrame(TRAP_ARMED);
    }
    
}

void Trap::processState() {
    unsigned int frame = _chandelierNode->getFrame();
    if (_timer >= 2) {
        _timer = 0;
        _chandelierNode->setFrame(frame);
        return;

    }
    if (getTriggered()) {
        if (frame < TRAP_TRIGGERING_END) ++frame;
    }
    CUAssertLog(frame <= TRAP_TRIGGERING_END, "Frame out of range.");
    ++_timer;
    _chandelierNode->setFrame(frame);
}

void Trap::update(float timestep) {
    GameEntity::update(timestep);
    if (getTriggered()) {
        if (_triggering > 0) {
            _triggering--;
        }
        if (!justTriggered()) {
            _node->getChildByName("radius")->setVisible(false);
        }
    }
    else if (getArmed()) {
        if (_arming > 0) {
            _arming--;
        }
        else if (doneArming()) {
            _node->getChildByName("radius")->setVisible(true);
        }
    }
    processState();
}

void Trap::dispose() {
    GameEntity::dispose();
    _node = nullptr;
    _chandelierNode = nullptr;
}

void Trap::reset() {
    GameEntity::reset();

    if (_chandelierNode != nullptr) {
        _chandelierNode->setFrame(0);
    }
}
