#pragma once
/**

This Battery class contains information about the battery's texture, whether
or not it should be drawn, and whether or not it needs to be deleted in the next 
collection phase

*/

#include <cugl/cugl.h>
#include "../GameEntity.h"
using namespace std;
using namespace cugl;

class Battery : public GameEntity {
private:
    /** Reference to the battery's sprite for drawing */
    shared_ptr<scene2::PolygonNode> _node;

public:
    /** Whether or not the battery should be drawn */
    bool drawable;

    /** Whether or not the battery needs to be deleted */
    bool deleteFlag;


    /** Flags the battery for deletion. */
    void destroy() {
        deleteFlag = true;
    }

    /** Flags the battery for deletion. */
    bool isDestroyed() {
        return deleteFlag;
    }

    static std::shared_ptr<Battery> alloc() {
        std::shared_ptr<Battery> result = std::make_shared<Battery>();
        return (result->init(Vec2::ZERO) ? result : nullptr);
    }

    /**
    * Returns a newly allocated Pal at the given position
    * @param pos Initial position in world coordinates
    *
    * @return a newly allocated Pal at the given position
    */
    static std::shared_ptr<Battery> alloc(const cugl::Vec2& pos) {
        std::shared_ptr<Battery> result = std::make_shared<Battery>();
        return (result->init(pos) ? result : nullptr);
    }


    /** Creates a Battery with the default values */
    Battery() : GameEntity() {}

    /** Releases all resources allocated with this Battery */
    ~Battery() { dispose(); }

    virtual void dispose() {
        GameEntity::dispose();
        _node = nullptr;
    };

    /**
     * @param node The trap node.
     * @param chandelier The chandelier node.
     */
    virtual void setNode(const shared_ptr<scene2::PolygonNode>& node);

    /** Initializes a new Battery at the given location */
    bool init(const Vec2& loc);

    /** Called when a battery on the floor collides with a Pal */
    void pickUp();

};