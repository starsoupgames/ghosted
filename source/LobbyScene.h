#pragma once
#ifndef __LOBBY_SCENE_H__
#define __LOBBY_SCENE_H__

#include <cugl/cugl.h>
#include "GameMode.h"
#include "NetworkController.h"

using namespace std;
using namespace cugl;

class LobbyScene : public GameMode {
private:
    shared_ptr<NetworkController> _network;

    bool _host;
    string _roomID;
    uint8_t _numPlayers;

    shared_ptr<scene2::Button> _start;

public:
    /**
     * Creates a new scene with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    LobbyScene() : GameMode(constants::GameMode::Lobby), _host(false), _roomID(""), _numPlayers(0) {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~LobbyScene() { dispose(); }

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

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    virtual void dispose() override;

    /**
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

    void setRoomID(string roomID) {
        _roomID = roomID;
    }
};

#endif /* __LOBBY_SCENE_H__ */
