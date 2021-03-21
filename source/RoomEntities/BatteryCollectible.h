#pragma once
/**

This Battery class contains information about the battery's texture, whether
or not it should be drawn, and whether or not it needs to be deleted in the next 
collection phase

*/

#include <cugl/cugl.h>
#include "GameEntity.h"
using namespace std;
using namespace cugl;

class BatteryCollectible : public GameEntity {
private:

	/** Reference to the battery's texture */
	shared_ptr<Texture> _batteryTexture;

public:
	/** Whether or not the battery should be drawn */
	bool drawable;

	/** Whether or not the battery needs to be deleted */
	bool deleteFlag;

	/** Returns the reference to the battery's texture */
	const shared_ptr<Texture> getTexture() const {
		return _batteryTexture;
	}

	/** Flags the battery for deletion. */
	void destroy() {
		deleteFlag = true;
	}

	/** Creates a Battery with the default values */
	BatteryCollectible() : GameEntity() {}

	/** Releases all resources allocated with this Battery */
	~BatteryCollectible() { dispose(); }

	/** Initializes a new Battery at the given location */
	bool init(const Vec2 position);

	/** Called when a battery on the floor collides with a Pal */
	void pickUp();

};