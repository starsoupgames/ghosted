#pragma once

#include <cugl/cugl.h>
#include "../GameEntity.h"
#include "../Utils.h"
using namespace std;
using namespace cugl;

/** Player Frame Sprite numbers */
#define IMG_LEFT 0   // Left idle frame
#define IMG_RIGHT 23   // Left idle frame
#define IMG_FRONT 24  // Front idle frame
#define IMG_BACK 48   // Back idle frame
#define IMG_LAST 71 // Last walk back frame

/** Player movement physics */
#define ACCELERATION 5
#define DRAG 2


/**
This class contains information about the Player.
*/
class Player : public GameEntity {
protected:
    /** Reference to the animation node */
    shared_ptr<scene2::AnimationNode> _node;

    /** Reference to the animation node */
    shared_ptr<scene2::PolygonNode> _shadow;

    /** Whether we are idle */
    bool _idle;

    /** Current player direction */
    Vec2 _direction;

    /** Current player velocity */
    Vec2 _velocity;

    /** Max speed */
    float _speed;

    /** Player id */
    unsigned id;

    /**frames since last change for animation fps**/
    int _timer;

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
        if (dir.isNearZero()) {
            _direction = Vec2::ZERO;
        }
        else {
            _direction = dir.normalize();
        }
    };

    /** Updates the Player velocity */
    void updateVelocity(Vec2 move) {
        // subtract drag
        if (_velocity.length() <= DRAG) {
            _velocity = Vec2::ZERO;
        }
        else {
            _velocity -= _velocity.normalize() * DRAG;
        }

        if (!move.isNearZero()) {
            // add acceleration
            _velocity += move.normalize() * ACCELERATION;

            // clamp to speed
            if (!_velocity.isNearZero() && _velocity.length() > _speed) {
                _velocity = _velocity.normalize() * _speed; // clamp to speed
            }
        }

        // set idle
        setIdle(_velocity.isNearZero());
    }

    /** Creates a Player with the default values */
    Player() : Player(5) {};

    Player(float speed) : GameEntity(), id(-1), _direction(Vec2(0.0f, -1.0f)), _speed(speed), _idle(true) {};

    /** Releases all resources allocated with this Player */
    ~Player() { dispose(); }

    /** Releases all resources allocated with this Player */
    void dispose();

    /** Initializes a new Player at the given location */
    virtual bool init(const Vec2& pos);

    /** Initializes a new Player at the origin */
    virtual bool init() { return init(Vec2::ZERO); }

    /** Get player type */
    virtual constants::PlayerType getType() {
        return constants::PlayerType::Undefined;
    }

    /**
     * @param node The Player node.
     * @param shadow The Shadow node.
     */
    virtual void setNode(const shared_ptr<scene2::AnimationNode>& node, const std::shared_ptr<scene2::PolygonNode>& shadow);

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
    
    /** Returns the current direction ENUM of the Player
     *
     * @return the current direction of the Player
     */
    uint8_t isDirection();

    /** Processes the direction for the animation and vision cone */
    virtual void processDirection() { };
    
    enum Direction {
        Keep = 0,
        Top = 1,
        Bottom = 2,
        Right = 3,
        Left = 4
    };
    
};
