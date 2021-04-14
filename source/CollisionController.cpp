#include "CollisionController.h"

using namespace cugl;
using namespace std;

/**
*  Handles collisions between a Pal and a Ghost, spooking the Pal if requirements for
*  spooking are fulfilled.
*
*  This method should be called only once per collision.
*
*  @param Ghost       The Ghost in the collision
*  @param Pal         The Pal in the collision
*/
void CollisionController::checkForCollision(const shared_ptr<Ghost>& ghost, const shared_ptr<Pal>& pal) {
    // Calculate the normal of the (possible) point of collision
    Vec2 norm = ghost->getLoc() - pal->getLoc();
    float distance = norm.length();
    float impactDistance = (ghost->getRadius() + pal->getRadius());

    if (!ghost->getTagged() && distance < impactDistance) {
        pal->setSpooked(true);
//		CULog("Spooked");
    }
}

/**
*  Checks for collisions between the Ghost and the Pal's vision, tagging the Ghost if
*  such a collision occurs.
*
*  This method should be called only once per collision.
*
*  @param Ghost       The Ghost in the collision
*  @param vision      The Pal's vision in the collision
*/
void CollisionController::checkForCollision(const shared_ptr<Ghost>& ghost, const shared_ptr<Pal>& pal, const shared_ptr<scene2::PolygonNode>& vision) {
     Poly2 node = vision->getPolygon() * vision->getTransform();
     auto M = vision->getTransform();
     //CULog("%f, %f", (vision->getBoundingRect() * M).getMidX(), (vision->getBoundingRect() * M).getMidY());


     if (node.contains(ghost->getLoc())) {
         //CULog("success");
         ghost->setTagged(true);
         ghost->setTimer(180);
     }
}

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
void CollisionController::checkForCollision(const shared_ptr<Pal>& pal, const shared_ptr<Battery>& battery) {
    // Calculate the normal of the (possible) point of collision
    Vec2 norm = battery->getLoc() - pal->getLoc();
    float distance = norm.length();
    float impactDistance = (battery->getRadius() + pal->getRadius());

    if (distance < impactDistance) {
        battery->dispose();
        pal->addBattery(1);
    }
}

/**
 *  Handles collisions between a Pal and a Trap, prompting the Trap to spook the pal if requirements are fulfilled
 *
 *  This method should be called only once per collision.
 *
 *  @param Pal        The Pal in the collision
 *  @param Battery    The Trap in the collision
 */
void CollisionController::checkForCollision(const shared_ptr<Pal>& pal, const shared_ptr<Trap>& trap) {
    Vec2 norm = trap->getLoc() - pal->getLoc();
    float distance = norm.length();    
    float thresh = 200;

    if (distance < thresh) {
        pal->setSpooked(true);
//        CULog("TRAP SPOOK");
    }
}

/**
* Nudge the entity to ensure it does not go out of bounds
*
* @param entity    The entity which may have collided
* @param bounds    The rectangular bounds of the playing field
*/
void CollisionController::checkInBounds(const std::shared_ptr<GameEntity>& entity, const cugl::Rect bounds) {
    Vec2 loc = entity->getLoc();
    // Turn this into referencing the entity's speed
    int adjust = 1;

    if (loc.x <= bounds.origin.x) {
        loc.x += adjust;
    }
    if (loc.x >= bounds.size.width + bounds.origin.x) {
        loc.x -= adjust;
    }
    if (loc.y <= bounds.origin.y) {
        loc.y += adjust;
    }
    if (loc.y >= bounds.size.height + bounds.origin.y) {
        loc.y -= adjust;
    }
    entity->setLoc(loc);
}

