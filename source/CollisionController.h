#pragma once
#ifndef __COLLISION_CONTROLLER_H__
#define __COLLISION_CONTROLLER_H__
#include <cugl/cugl.h>
#include "GameEntity.h"
#include "GameEntities/Players/PlayerGhost.h"
#include "GameEntities/Players/PlayerPal.h"
#include "GameEntities/BatteryCollectible.h"
#include "RoomEntities/Trap.h"

/**
 * Namespace of functions implementing simple game physics.
 */
class CollisionController {
private:

public:
#pragma mark Constructors
    /**
    * Creates a new collision controller with the default settings
    *
    * To use this controller, you will need to initialize it first
    */
    CollisionController() {};

    /**
     * Disposes this collision controller, releasing all resources.
     */
    ~CollisionController() {};

#pragma mark -
#pragma mark Collision Handling

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
    *  Checks for collisions between the Ghost and the Pal's vision, tagging the Ghost if
    *  such a collision occurs.
    *
    *  This method should be called only once per collision.
    *
    *  @param Ghost       The Ghost in the collision
    *  @param vision      The Pal's vision in the collision
    */
    void checkForCollision(const shared_ptr<Ghost>& ghost, const shared_ptr<Pal>& pal, const shared_ptr<scene2::PolygonNode>& vision);

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
     *  Handles collisions between a Pal and a Trap, prompting the Trap to spook the pal if requirements are fulfilled
     *
     *  This method should be called only once per collision.
     *
     *  @param Pal        The Pal in the collision
     *  @param Battery    The Trap in the collision
     */
    void checkForCollision(const shared_ptr<Pal>& pal, const shared_ptr<Trap>& trap);


    /**
    * Nudge the entity to ensure it does not go out of bounds
    *
    * @param entity    The entity which may have collided
    * @param bounds    The rectangular bounds of the playing field
    */
    void checkInBounds(const std::shared_ptr<GameEntity>& entity, const cugl::Rect bounds);


    /**
     * Processes the start of a collision
     *
     * This method is called when we first get a collision between two objects.
     * We use this method to test if it is the "right" kind of collision.  In
     * particular, we use it to test if we make it to the win door.
     *
     * @param  contact  The two bodies that collided
     */
    void beginContact(b2Contact* contact);


    /**
     * Handles any modifications necessary before collision resolution
     *
     * This method is called just before Box2D resolves a collision.  We use
     * this method to implement sound on contact, using the algorithms outlined
     * in Ian Parberry's "Introduction to Game Physics with Box2D".
     *
     * @param  contact  The two bodies that collided
     * @param  contact  The collision manifold before contact
     */
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);

#pragma mark -
};

#endif /* __COLLISION_CONTROLLER_H__ */
