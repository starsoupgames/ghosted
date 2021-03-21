#pragma once
/**
This Obstacle class contains information about the Obstacle's texture
*/

#include <cugl/cugl.h>
#include "RoomEntity.h"
using namespace std;
using namespace cugl;

class Obstacle : public RoomEntity {
private:

	/** Reference to the Obstacle's texture */
	shared_ptr<Texture> _obsTexture;

public:

	/** Returns the reference to the obstacle's texture */
	const shared_ptr<Texture> getTexture() const {
		return _obsTexture;
	}

	/** Creates a Obstacle with the default values */
	Obstacle() : RoomEntity() {}

	/** Releases all resources allocated with this Obstacle */
	~Obstacle() { dispose(); }

	/** Initializes a new Obstacle at the given position */
	bool init(const Vec2 position);
};