#include "Trap.h"

using namespace cugl;

void Trap::setNode(const shared_ptr<scene2::PolygonNode>& node, const std::shared_ptr<scene2::AnimationNode>& chandelier, const std::shared_ptr<scene2::AnimationNode>& smoke) {
    _node = node;
    _chandelierNode = chandelier;
    if (_chandelierNode != nullptr) {
        //_chandelierNode->setPosition(node->getNodeToWorldTransform().transformVector((Vec2(node->getWidth(), node->getHeight()) * 2)));
        _timer = 0;
        _chandelierNode->setFrame(TRAP_ARMED);
    }
    _smokeNode = smoke;
    if (_smokeNode != nullptr) {
        _smokeNode->setFrame(SMOKE_START);
        _smokeNode->setVisible(false);
    }
}

void Trap::processState() {
    unsigned int frame = _chandelierNode->getFrame();
    unsigned int smokeFrame = _smokeNode->getFrame();
    if (_timer >= 2) {
        _timer = 0;
        _chandelierNode->setFrame(frame);
        return;

    }
    if (getTriggered()) {
        if (frame < TRAP_TRIGGERING_END) ++frame;
        if (frame >= SMOKE_FRAME_START) {
            if (smokeFrame == 0) {
                _smokeNode->setVisible(true);
                ++smokeFrame;
            }
            else if (smokeFrame < SMOKE_END) {
               ++smokeFrame;
            }
            else {
                _smokeNode->setVisible(false);
            }
        }
    }
    CUAssertLog(frame <= TRAP_TRIGGERING_END, "Frame out of range.");
    ++_timer;
    _smokeNode->setFrame(smokeFrame);
    _chandelierNode->setFrame(frame);
}

void Trap::update(float timestep) {
    GameEntity::update(timestep);
    if (getTriggered()) {
        if (_triggering > 0) {
            _triggering--;
        }
        if (doneTriggering()) {
            _node->setVisible(false);
            _chandelierNode->setVisible(false);
        }
        else if (!justTriggered()) {
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
    if (_node != nullptr) {
        _node->dispose();
    }
    _node = nullptr;
    if (_chandelierNode != nullptr) {
        _chandelierNode->dispose();
    }
    _chandelierNode = nullptr;
}

void Trap::reset() {
    GameEntity::reset();

    if (_chandelierNode != nullptr) {
        _chandelierNode->setFrame(0);
    }
}
