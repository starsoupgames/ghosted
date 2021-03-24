#pragma once
#ifndef __COLLISION_CONTROLLER_H__
#define __COLLISION_CONTROLLER_H__
#include <cugl/cugl.h>
#include "GameEntity.h"
#include "GameEntities/Players/PlayerGhost.h"
#include "GameEntities/Players/PlayerPal.h"
#include "GameEntities/BatteryCollectible.h"

/**
 * Namespace of functions implementing simple game physics.
 */
class CollisionController {
private:
	bool checkSpooked(const shared_ptr<Ghost>& ghost, const shared_ptr<Pal>& pal);

public:
	/**
	*  Handles collisions between a Pal and a Ghost, spooking the Pal if requirements for
	*  spooking are fulfilled.
	*
	*  This method should be called only once per collision.
	*
	*  @param Ghost       The Ghost in the collision
	*  @param Pal         The Pal in the collision
	*/
	void checkForCollision(const shared_ptr<Ghost>& ghost, const shared_ptr<Pal>& pal);


	/**
	 *  Handles collisions between a Pal and a Battery, prompting the Pal to pick up the
	 *	Battery if requirements are fulfilled
	 *
	 *	CURRENTLY THE PAL WILL AUTOMATICALLY PICK UP THE BATTERY
	 *
	 *  This method should be called only once per collision.
	 *
	 *  @param Pal        The Pal in the collision
	 *  @param Battery    The Battery in the collision
	 */
	void checkForCollision(const shared_ptr<Pal>& pal, const shared_ptr<Battery>& battery);


	/**
	* Nudge the entity to ensure it does not go out of bounds
	*
	* @param entity    The entity which may have collided
	* @param bounds    The rectangular bounds of the playing field
	*/
	void checkInBounds(const std::shared_ptr<GameEntity>& entity, const cugl::Rect bounds);
};

#endif /* __COLLISION_CONTROLLER_H__ */