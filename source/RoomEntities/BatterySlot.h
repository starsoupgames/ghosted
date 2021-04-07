#pragma once
/** 
This BatterySlot class contains information about the BatterySlot's texture and level of charge
*/

#include <cugl/cugl.h>
#include "RoomEntity.h"
using namespace std;
using namespace cugl;

class BatterySlot : public RoomEntity {
private:

	/** Reference to the Battery Slot's texture */
	shared_ptr<Texture> _slotTexture;

	/** Level of charge remaining */
	float _charge;

public:


	/** Returns the reference to the battery's texture */
	const shared_ptr<Texture> getTexture() const {
		return _slotTexture;
	}

	/** Returns the amount of charge the battery has */
	float getCharge() const {
		return _charge;
	}
    
    /** Returns the amount of charge the battery has */
    void setCharge() {
        _charge = 5;
    }

	/** Updates the battery slot's charge */
	void update(float timestep);

	/** Creates a BatterySlot with the default values */
	BatterySlot() : RoomEntity(), _charge(0) {}

	/** Releases all resources allocated with this Battery Slot */
	~BatterySlot() { dispose(); }

	/** Initializes a new BatterySlot at the given position */
	bool init(const Vec2 position);
    
    void setTextures(const std::shared_ptr<Texture>& trap, Size size);
    
    /**
    * @return a newly allocated Trap at the origin.
    */
    static shared_ptr<BatterySlot> alloc() {
        shared_ptr<BatterySlot> result = make_shared<BatterySlot>();
        return (dynamic_pointer_cast<RoomEntity>(result)->init() ? result : nullptr);
    }

    static shared_ptr<BatterySlot> alloc(const Vec2& pos) {
        shared_ptr<BatterySlot> result = make_shared<BatterySlot>();
        return (dynamic_pointer_cast<RoomEntity>(result)->init(pos, 3) ? result : nullptr);
    };
    
    void dispose();
    
    void reset();
};
