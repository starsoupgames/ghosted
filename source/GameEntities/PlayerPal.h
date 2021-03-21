#pragma once
/**

This PlayerPal class contains information about the Pal's texture, the amount of batteries it has,
and methods for its actions

*/

#include <cugl/cugl.h>
#include "GameEntity.h"
using namespace std;
using namespace cugl;

class Pal : public GameEntity {
private:

	/** Which Pal this is */
	int _id;

	/** Amount of Traps left */
	int _batteries;

	/** Whether a Pal is spooked or not */
	bool _spooked;

	/** Reference to the Pal's texture */
	shared_ptr<Texture> _palTexture;

public:

	/** Returns the id of the Pal */
	int getId() const {
		return _id;
	}

	/** Returns the amount of batteries left */
	int getBatteries() const {
		return _batteries;
	}

	/** Returns whether or not a Pal is spooed */
	int getSpooked() const {
		return _spooked;
	}

	/** Returns the reference to the battery's texture */
	const shared_ptr<Texture> getTexture() const {
		return _palTexture;
	}

	/** Sets the amount of traps left */
	void setTraps(int num) {
		_batteries = num;
	}

	/** Sets whether or not a Pal is spooked */
	void setSpooked(bool spooked) {
		_spooked = spooked;
	}

	/** Creates a Pal with the default values */
	Pal() : GameEntity() {};

	/** Releases all resources allocated with this Pal */
	~Pal() { dispose(); }

	/** Initializes a new Pal at the given location with the given id */
	bool init(int x, int y, int id);

	/** Processes the animation and vision cone to process a turn */
	void processTurn(float turn);

	/** Moves the Pal in the corresponding direction */
	void move(Vec2 direction);

	/** Places a battery in the nearby slot */
	void placeBattery();

};