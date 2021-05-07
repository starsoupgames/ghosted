#pragma once
/**
This Trap class contains information about the Trap's texture, whether or not its armed
*/

#include <cugl/cugl.h>
#include "../GameEntity.h"

#define ARMING_TICKS 100
#define TRIGGERING_TICKS 200
#define TRAP_DURATION 30

using namespace std;
using namespace cugl;

class Trap : public GameEntity {
private:
    /** Progress while arming */
    int _arming;

    /** Progress while triggering */
    int _triggering;

    /** Done triggering */
    bool _doneTriggering;

public:

    /** Returns whether or not the trap is armed */
    bool getArmed() const {
        return _arming >= 0;
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
           _animationNode->setColor(Color4f::RED); // TEMP
        }
    }

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
        return (result->init(pos, 5) ? result : nullptr);
    };

    void update(float timestep);
    
    void dispose();
    
    void reset();
};
