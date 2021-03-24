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

class GameScene : public cugl::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    InputController _input;
    /** Controller for handling collisions */
    CollisionController _collision;
    /** Controller for handling networking */
    NetworkController _network;

    // VIEW
    /** Root node */
    std::shared_ptr<cugl::scene2::SceneNode> _root;
    std::shared_ptr<cugl::scene2::AnimationNode> _palNode;
    std::shared_ptr<cugl::scene2::AnimationNode> _ghostNode;

    // MODEL
    std::shared_ptr<Pal> _palModel;
    std::shared_ptr<Ghost> _ghostModel;

    /** Room ID for networking */
    string _roomID;
    bool _host;

    unsigned _mode;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : cugl::Scene2(), _mode(2), _host(false) {}

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
    void dispose();

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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);

    unsigned getMode() {
        return _mode;
    };

    void setRoomID(string roomID) {
        _roomID = roomID;
    }
};

#endif /* __GHOSTED_GAME_SCENE_H__ */