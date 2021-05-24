#pragma once
/**
This Trap class contains information about the Trap's texture, whether or not its armed
*/

#include <cugl/cugl.h>
#include "../GameEntity.h"

#define ARMING_TICKS 100
#define TRIGGERING_TICKS 200
#define TRAP_DURATION 30
#define TRAP_ARMED 0
#define TRAP_TRIGGERING_END 18
#define SMOKE_FRAME_START 10
#define SMOKE_START 0
#define SMOKE_END 7


using namespace std;
using namespace cugl;

class Trap : public GameEntity {
private:
    /** Reference to the trap's sprite for drawing */
    shared_ptr<scene2::PolygonNode> _node;

    /** Reference to the trap's chandelier sprite for drawing */
    shared_ptr<scene2::AnimationNode> _chandelierNode;

    /** Reference to the trap's smoke sprite for drawing */
    shared_ptr<scene2::AnimationNode> _smokeNode;

    /** Progress while arming */
    int _arming;

    /** Progress while triggering */
    int _triggering;

    /** Done triggering */
    bool _doneTriggering;

    /**frames since last change for animation fps**/
    int _timer;

public:

    /** Returns whether or not the trap is armed */
    bool getArmed() const {
        return _arming >= 0;
    }
    
    bool justArmed() const {
        return _arming > ARMING_TICKS - TRAP_DURATION;
    }

    /** Sets whether or not the trap is done triggering */
    bool doneArming() const {
        return _arming == 0;
    }
    
    /** Returns whether or not the trap has been triggered */
    bool getTriggered() const {
        return _triggering >= 0;
    }

    bool justTriggered() const {
        return _triggering > TRIGGERING_TICKS - TRAP_DURATION;
    }

    /** Sets whether or not the trap is done triggering */
    bool doneTriggering() const {
        return _triggering == 0;
    }

    /** Arms the trap */
    void setArmed() {
        if (!getArmed()) {
            _arming = ARMING_TICKS;
        }
    }
    
    /** Sets whether or not the trap has been triggered */
    void setTriggered() {
        if (doneArming() && !getTriggered()) {
            _triggering = TRIGGERING_TICKS;
        }
    }

    /** Processes the frame for the animation */
    virtual void processState();

    /** Creates a Trap with the default values */
    Trap() : GameEntity(), _arming(-1), _triggering(-1), _doneTriggering(false) {}

    /** Releases all resources allocated with this Trap */
    ~Trap() { dispose(); }
        
    /**
    * @return a newly allocated Trap at the origin.
    */
    static shared_ptr<Trap> alloc() {
        shared_ptr<Trap> result = make_shared<Trap>();
        return (result->init() ? result : nullptr);
    }

    static shared_ptr<Trap> alloc(const Vec2& pos) {
        shared_ptr<Trap> result = make_shared<Trap>();
        return (result->init(pos) ? result : nullptr);
    };

    /**
     * @param node The trap node.
     * @param chandelier The chandelier node.
     */
    void setNode(const shared_ptr<scene2::PolygonNode>& node, const std::shared_ptr<scene2::AnimationNode>& chandelier, const std::shared_ptr<scene2::AnimationNode>& smoke);

    void update(float timestep);
    
    void dispose();
    
    void reset();
};
