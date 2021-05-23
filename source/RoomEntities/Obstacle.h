#pragma once
#ifndef __OBSTACLE_H__
#define __OBSTACLE_H__

#include <cugl/cugl.h>
#include "../RoomEntity.h"
using namespace std;
using namespace cugl;

class Obstacle : public RoomEntity {
private:

public:

	/** Creates a Obstacle with the default values */
	Obstacle() : RoomEntity() {}

	/** Releases all resources allocated with this Obstacle */
	~Obstacle() { dispose(); }

	/** Initializes a new Obstacle at the given position */
	bool init(const Vec2 position);
};

#endif