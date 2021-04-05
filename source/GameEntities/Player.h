#pragma once

#include <cugl/cugl.h>
#include "../GameEntity.h"
using namespace std;
using namespace cugl;

/**
This class contains information about the Player.
*/
class Player : public GameEntity {
protected:
    /** Reference to the animation node */
    shared_ptr<scene2::AnimationNode> _node;

    /** Whether we are idle */
    bool _idle;

    /** Current player direction */
    Vec2 _direction;

    /** Current Player movement */
    Vec2 _move;

public:

    /** Returns the reference to the animation node */
    const shared_ptr <scene2::AnimationNode> getNode() const {
        return _node;
    }
    
    /** Returns the current direction VECTOR of the Player
     *
     * @return the current direction vector for the Player
     */
    Vec2 getDir() {
        return _direction;
    };
    
    /** Sets the Player direction */
    void setDir(Vec2 dir) {
        _direction = dir.normalize();
    };

    /** Creates a Player with the default values */
    Player() : GameEntity(), _direction(Vec2(0.0f, -1.0f)) {};

    /** Releases all resources allocated with this Player */
    ~Player() { dispose(); }

    /** Releases all resources allocated with this Player */
    void dispose();

    /** Initializes a new Player at the given location */
    bool init(const Vec2& pos);

    bool init() { return init(Vec2::ZERO); }

    /** Moves the Player */
    void move(Vec2 move) {
        _move = move.normalize();
    }

    /** Get player type */
    virtual uint8_t getType() {
        return Type::Undefined;
    }

    /**
     * @param value The Player node.
     */
    void setNode(const shared_ptr<scene2::AnimationNode>& value);

    /**
     * Updates the state of the model
     *
     * This method moves the Player forward, dampens the forces (if necessary)
     * and updates the sprite if it exists.
     *
     * @param timestep  Time elapsed since last called.
     */
    virtual void update(float timestep = 0.0f);

    /**
     * Resets the Player back to its original settings
     */
    void reset();

    enum Type {
        Undefined = 0,
        Pal = 1,
        Ghost = 2
    };
    
    /** Returns the current direction ENUM of the Player
     *
     * @return the current direction of the Player
     */
    uint8_t isDirection() {
        uint8_t dir = Direction::Bottom;
        float minDist = _direction.distance(Vec2(0, -1));
        
        if (_direction.distance(Vec2(0, 1)) < minDist) {
            dir = Direction::Top;
            minDist = _direction.distance(Vec2(0, 1));
        }
        if (_direction.distance(Vec2(1, 0)) < minDist) {
            dir = Direction::Right;
            minDist = _direction.distance(Vec2(1, 0));
        }
        if (_direction.distance(Vec2(-1, 0)) < minDist) {
            dir = Direction::Left;
        }
        return dir;
    }
    
    enum Direction {
        Keep = 0,
        Top = 1,
        Bottom = 2,
        Right = 3,
        Left = 4
    };
    
};
