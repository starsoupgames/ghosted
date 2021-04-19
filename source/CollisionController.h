#pragma once
#ifndef __COLLISION_CONTROLLER_H__
#define __COLLISION_CONTROLLER_H__
#include <cugl/cugl.h>
#include "GameEntity.h"
#include "GameMap.h"

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
    ~CollisionController() { dispose(); };

    bool init();

    /**
    * Disposes of all (non-static) resources allocated to this controller.
    */
    virtual void dispose();

    /**
     * Allocates this collision controller
     */
    static shared_ptr<CollisionController> alloc() {
        shared_ptr<CollisionController> result = make_shared<CollisionController>();
        return (result->init() ? result : nullptr);
    }

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
