#include "CollisionController.h"

using namespace cugl;
using namespace std;

void CollisionController::dispose() {
    _gameMap = nullptr;
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
void CollisionController::beginContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();

    // Must handle ghost and vision cone tagging
}

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
void CollisionController::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    float speed = 0;
}