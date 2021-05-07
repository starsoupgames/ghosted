#pragma once
#ifndef __WIN_SCENE_H__
#define __WIN_SCENE_H__

#include <cugl/cugl.h>
#include "GameMode.h"
#include "NetworkController.h"

using namespace std;
using namespace cugl;

class WinScene : public GameMode {
private:
    shared_ptr<NetworkController> _network;

    /** Escape to start screen */
    shared_ptr<scene2::Button> _quit;
    
    /** Pals */
    shared_ptr<scene2::SceneNode> _doe;
    shared_ptr<scene2::SceneNode> _seal;
    shared_ptr<scene2::SceneNode> _tanuki;
    
    shared_ptr<scene2::SceneNode> _ghost;
    
    /** Winner of the game */
    constants::PlayerType _winner;

public:
    /**
     * Creates a new scene with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    WinScene() : GameMode(constants::GameMode::Win), _winner(constants::PlayerType::Undefined) {};
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~WinScene() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    virtual void dispose() override;
    
    /**
     * Initializes the controller contents, making it ready for loading
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
    
    /**
     * The method called to update the game mode.
     *
     * This method updates the progress bar amount.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
    
    /**
     * Sets whether the scene is currently active
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;


    void setNetwork(shared_ptr<NetworkController> network) {
        _network = network;
    }
    
    /**
     * Updates the winner for the win screen
     */
    void setWinner(constants::PlayerType playerWon) {
        _winner = playerWon;
    }
};

#endif /* __WIN_SCENE_H__ */
