#ifndef __GHOSTED_GAME_SCENE_H__
#define __GHOSTED_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include <iostream>
#include <sstream>
#include "GameEntities/Players/PlayerPal.h"
#include "GameEntities/Players/PlayerGhost.h"
#include "InputController.h"
#include "CollisionController.h"
#include "NetworkController.h"
#include "NetworkData.h"

using namespace std;
using namespace cugl;

class GameScene : public Scene2 {
protected:
    /** The asset manager for this game mode. */
    shared_ptr<AssetManager> _assets;

    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    InputController _input;
    /** Controller for handling collisions */
    CollisionController _collision;
    /** Controller for handling networking */
    NetworkController _network;

    // VIEW
    /** Root node */
    shared_ptr<scene2::SceneNode> _root;
    shared_ptr<scene2::AnimationNode> _palNode;
    shared_ptr<scene2::AnimationNode> _ghostNode;
    shared_ptr<scene2::PolygonNode> _visionNode;

    // MODEL
    shared_ptr<NetworkData> _networkData;
    vector<shared_ptr<Player>> _players;
    shared_ptr<Pal> _palModel;
    shared_ptr<Ghost> _ghostModel;

    /** Room ID for networking */
    string _roomID;
    bool _host;

    uint8_t _mode;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : Scene2(), _host(false), _roomID(""), _mode(2) {}

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

    uint8_t getMode() {
        return _mode;
    };

    void setRoomID(string roomID) {
        _roomID = roomID;
    }
};

#endif /* __GHOSTED_GAME_SCENE_H__ */
