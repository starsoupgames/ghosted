#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "GameMode.h"
#include "Constants.h"
#include "GameMap.h"
#include "GameEntities/Players/PlayerPal.h"
#include "GameEntities/Players/PlayerGhost.h"
#include "InputController.h"
#include "CollisionController.h"
#include "NetworkController.h"
#include "NetworkData.h"
#include "RoomEntities/Trap.h"
#include "RoomEntities/BatterySlot.h"


using namespace std;
using namespace cugl;

class GameScene : public GameMode {
protected:
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    shared_ptr<InputController> _input;
    /** Controller for handling collisions */
    shared_ptr<CollisionController> _collision;
    /** Controller for handling networking */
    shared_ptr<NetworkController> _network;

    // VIEW
    /** Root node */
    shared_ptr<scene2::OrderedNode> _root;
    shared_ptr<scene2::OrderedNode> _topRoot;
    shared_ptr<scene2::OrderedNode> _dimRoot;
    shared_ptr<scene2::OrderedNode> _litRoot;
    shared_ptr<scene2::PolygonNode> _floorNode;
    shared_ptr<scene2::PolygonNode> _litFloorNode;
    /** Reference to the debug root of the scene graph */
    shared_ptr<scene2::SceneNode> _debugNode;
    /** Reference to the win and lose message labels */
    shared_ptr<scene2::Label> _ghostWinNode;
    shared_ptr<scene2::Label> _palWinNode;

    /**offset for flashlight position**/
    //Vec2 _flashlightOffset = Vec2(0, -50);
    
    /** Reference to all the nodes for the rooms */
    vector<shared_ptr<scene2::PolygonNode>> _roomNodes;

    /** The polygon node representing the vision cone */
    shared_ptr<scene2::PolygonNode> _visionNode;

    /** The Box2D world */
    shared_ptr<physics2::ObstacleWorld> _world;

    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

    // MODEL
    shared_ptr<GameMap> _gameMap;
    vector<shared_ptr<Player>> _players;

    /** Whether or not debug mode is active */
    bool _debug;

public:
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : GameMode(constants::GameMode::Game), _scale(1), _debug(false) {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    virtual void dispose() override;

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const shared_ptr<AssetManager>& assets);


#pragma mark -
#pragma mark State Access
    /**
   * Returns true if debug mode is active.
   *
   * If true, all objects will display their physics bodies.
   *
   * @return true if debug mode is active.
   */
    bool isDebug() const { return _debug; }

    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) { _debug = value; _debugNode->setVisible(value); }

    /**
    * Sets the pointer to the network.
    */
    void setNetwork(shared_ptr<NetworkController> network) {
        _network = network;
    }

    /**
     * Sets the pointer to the input controller
     */
    void setInput(shared_ptr<InputController> input) {
        _input = input;
    }

    /**
     * Sets the pointer to the collision controller
     */
    void setCollision(shared_ptr<CollisionController> collision) {
        _collision = collision;
    }

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
    
    /**
     * Sets the cone position based on player direction
     */
    void updateVision(const std::shared_ptr<Player>& player);

    /**
     * Resets the gamescene
     */
    void reset();

    void draw(const std::shared_ptr<SpriteBatch>& batch, const std::shared_ptr<SpriteBatch>& shaderBatch);
};

#endif /* __GAME_SCENE_H__ */
