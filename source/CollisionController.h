#pragma once
#ifndef __COLLISION_CONTROLLER_H__
#define __COLLISION_CONTROLLER_H__
#include <cugl/cugl.h>
#include "GameEntity.h"
#include "GameMap.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>

/**
 * Namespace of functions implementing simple game physics.
 */
class CollisionController {
private:
    /** The GameMap Model */
    shared_ptr<GameMap> _gameMap;

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
    ~CollisionController() { dispose(); };

    /**
    * Disposes of all (non-static) resources allocated to this controller.
    */
    virtual void dispose();

#pragma mark -
#pragma mark State Access

    /**
    * Sets the GameMap model
    *
    * @param  gameMap  The pointer to the GameMap
    */
    void setGameMap(shared_ptr<GameMap> gameMap) { _gameMap = gameMap; }

#pragma mark -
#pragma mark Collision Handling


    /**
     * Processes the start of a collision
     *
     * This method is called when we first get a collision between two objects.
     * We use this method to test if it is the "right" kind of collision.  In
     * particular, we use it to handle interactions between entities.
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
