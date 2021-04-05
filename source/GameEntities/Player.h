#pragma once

#include <cugl/cugl.h>
#include "../GameEntity.h"
using namespace std;
using namespace cugl;

/** Player Frame Sprite numbers */
#define IMG_RIGHT 0   // Right idle frame
#define IMG_LEFT 21   // Left idle frame
#define IMG_FRONT 42  // Front idle frame
#define IMG_BACK 63   // Back idle frame
#define IMG_LAST 83

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

    /** Player id */
    unsigned id;

public:

    /** Returns the reference to the animation node */
    const shared_ptr <scene2::AnimationNode> getNode() const {
        return _node;
    }

    /** Sets whether the Player is idle */
    void setIdle(bool idle) {
        _idle = idle;
    }
    
    /**
     * @return the current direction vector for the Player
     */
    Vec2 getDir() {
        return _direction;
    };
    
    /** Sets the Player direction */
    void setDir(Vec2 dir) {
        _direction = dir.normalize();
    };

    /** Sets the Player movement */
    void setMove(Vec2 move) {
        _move = move.normalize();
    }

    /** Creates a Player with the default values */
    Player() : GameEntity(), id(-1), _direction(Vec2(0.0f, -1.0f)) {};

    /** Releases all resources allocated with this Player */
    ~Player() { dispose(); }

    /** Releases all resources allocated with this Player */
    void dispose();

    /** Initializes a new Player at the given location */
    bool init(const Vec2& pos);

    bool init() { return init(Vec2::ZERO); }

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

    /** Processes the direction for the animation and vision cone */
    virtual void processDirection();
    
    enum Direction {
        Keep = 0,
        Top = 1,
        Bottom = 2,
        Right = 3,
        Left = 4
    };
    
};
