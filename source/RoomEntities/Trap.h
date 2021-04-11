#pragma once
/**
This Trap class contains information about the Trap's texture, whether or not its armed
*/

#include <cugl/cugl.h>
#include "../RoomEntity.h"

#define TRAP_RADIUS 5

using namespace std;
using namespace cugl;

class Trap : public RoomEntity {
private:

	/** Whether or not the trap is armed */
	bool _armed;
    
    bool _triggered;

	/** Reference to the Trap's texture */
	shared_ptr<Texture> _trapTexture;

public:

	/** Returns whether or not the trap is armed */
	bool getArmed() const {
		return _armed;
	}
    
    /** Returns whether or not the trap has been triggered */
    bool getTriggered() const {
        return _triggered;
    }

	/** Returns the reference to the battery's texture */
	const shared_ptr<Texture> getTexture() const {
		return _trapTexture;
	}
    
    void setTextures(const std::shared_ptr<Texture>& trap, Size size);

	/** Sets whether or not the trap is armed */
	void setArmed(bool armed) {
		_armed = armed;
	}
    
    /** Sets whether or not the trap has been triggered */
    void setTriggered(bool triggered) {
        if (_armed) {
            _triggered = triggered;
        }
    }

	/** Updates the battery slot's charge */
	void update(float timestep);

	/** Creates a Trap with the default values */
	Trap() : RoomEntity(), _armed(false), _triggered(false) {}

	/** Releases all resources allocated with this Battery Slot */
	~Trap() { dispose(); }
        
    /**
    * @return a newly allocated Trap at the origin.
    */
    static shared_ptr<Trap> alloc() {
        shared_ptr<Trap> result = make_shared<Trap>();
        return (dynamic_pointer_cast<RoomEntity>(result)->init() ? result : nullptr);
    }

    static shared_ptr<Trap> alloc(const Vec2& pos) {
        shared_ptr<Trap> result = make_shared<Trap>();
        return (dynamic_pointer_cast<RoomEntity>(result)->init(pos, TRAP_RADIUS) ? result : nullptr);
    };
    
    void dispose();
    
    void reset();
};
