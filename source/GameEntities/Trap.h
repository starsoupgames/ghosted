#pragma once
/**
This Trap class contains information about the Trap's texture, whether or not its armed
*/

#include <cugl/cugl.h>
#include "RoomEntity.h"
using namespace std;
using namespace cugl;

class Trap : public RoomEntity {
private:

	/** Whether or not the trap is armed */
	bool _armed;

	/** Reference to the Trap's texture */
	shared_ptr<Texture> _trapTexture;

public:

	/** Returns whether or not the trap is armed */
	bool getArmed() const {
		return _armed;
	}

	/** Returns the reference to the battery's texture */
	const shared_ptr<Texture> getTexture() const {
		return _trapTexture;
	}

	/** Sets whether or not the trap is armed */
	void setArmed(bool armed) {
		_armed = armed;
	}

	/** Updates the battery slot's charge */
	void update();

	/** Creates a Trap with the default values */
	Trap() : RoomEntity() {}

	/** Releases all resources allocated with this Battery Slot */
	~Trap() { dispose(); }

	/** Initializes a new Trap at the given position */
	bool init(const Vec2 position);
};