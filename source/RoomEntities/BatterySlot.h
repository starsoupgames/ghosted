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
	int _charge;

public:


	/** Returns the reference to the battery's texture */
	const shared_ptr<Texture> getTexture() const {
		return _slotTexture;
	}

	/** Returns the amount of charge the battery has */
	int getCharge() const {
		return _charge;
	}

	/** Updates the battery slot's charge */
	void update();

	/** Creates a BatterySlot with the default values */
	BatterySlot() : RoomEntity() {}

	/** Releases all resources allocated with this Battery Slot */
	~BatterySlot() { dispose(); }

	/** Initializes a new BatterySlot at the given position */
	bool init(const Vec2 position);
};