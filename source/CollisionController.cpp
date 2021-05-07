#include "CollisionController.h"

using namespace cugl;
using namespace std;

#pragma mark Constructors
bool CollisionController::init() {
    return true;
}

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

    // Handles ghost and pal spooking
    bool check1 = body1->GetUserData() != _gameMap->getGhost().get();
    bool check2 = body2->GetUserData() == _gameMap->getGhost().get();
    if (check1 && check2) {
        shared_ptr<Player> palModel = _gameMap->getModel(body1);
        dynamic_pointer_cast<Pal>(palModel)->setSpooked(!dynamic_pointer_cast<Ghost>(_gameMap->getGhost())->getTagged());
    }

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