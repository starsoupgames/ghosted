#pragma once
/**

This PlayerGhost class contains information about the Ghost's texture, the amount of traps it has left,
and methods for the Ghost's actions

*/

#include <cugl/cugl.h>
#include "GameEntity.h"
using namespace std;
using namespace cugl;

class Ghost : public GameEntity {
private:

	/** Amount of Traps left */
	int _traps;

	/** Reference to the Ghost's texture */
	shared_ptr<Texture> _ghostTexture;

public:

	/** Returns the amount of traps left */
	int getTraps() const {
		return _traps;
	}

	/** Returns the reference to the battery's texture */
	const shared_ptr<Texture> getTexture() const {
		return _ghostTexture;
	}

	/** Sets the amount of traps left */
	void setTraps(int num) {
		_traps = num;
	}

	/** Creates a Ghost with the default values */
	Ghost() : GameEntity() {};

	/** Releaes all resources allocated with this Ghost */
	~Ghost() { dispose(); }

	/** Initializes a new Ghost at the given location */
	bool init(int x, int y);

	/** Moves the Ghost in the corresponding direction */
	void move(Vec2 direction);

	/** Triggers the nearest trap */
	void trigger();
};