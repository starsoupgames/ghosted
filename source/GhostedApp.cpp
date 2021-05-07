//
//  GhostedApp.cpp
//  Cornell University Game Library (CUGL)
//
//  This is the implementation file for the custom application. This is the
//  definition of your root (and in this case only) class.
//
//  CUGL zlib License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 1/8/17
//
// Include the class header, which includes all of the CUGL classes
#include "GhostedApp.h"



// This keeps us from having to write cugl:: all the time
using namespace cugl;

const std::string _vsource =
#include "../assets/shaders/lightShader.vert"
;
const std::string _fsource =
#include "../assets/shaders/lightShader.frag"
;

/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void GhostedApp::onStartup() {
    Size size = getDisplaySize();
    // Create an asset manager to load all assets
    _assets = AssetManager::alloc();
    _mode = constants::GameMode::Loading;
    
    buildScene();
    buildShader();

    // Create a sprite batch (and background color) to render the scene
    
    _batch = SpriteBatch::alloc();
    _shaderBatch = SpriteBatch::alloc(_shader);
    setClearColor(Color4::RED);
    
    _resetPressed = false;

    // Activate mouse or touch screen input as appropriate
    // We have to do this BEFORE the scene, because the scene has a button
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif
    Input::activate<TextInput>();
    Input::activate<Keyboard>();
    Application::onStartup();
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void GhostedApp::onShutdown() {
    // Delete all smart pointers
    _batch = nullptr;
    _assets = nullptr;

    _shader = nullptr;
    _shaderBatch = nullptr;

    _network = nullptr;
    _networkData = nullptr;
    _input = nullptr;
    _collision = nullptr;
    
    _loadKeys = false;
    
    // Deativate input
#if defined CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    Input::deactivate<TextInput>();
    Input::deactivate<Keyboard>();
    Application::onShutdown();
}

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GhostedApp::update(float timestep) {
    constants::GameMode mode = constants::GameMode::Loading;
    if (_loading.isActive()) {
        mode = constants::GameMode::Loading;
    }
    else if (_start.isActive()) {
        mode = _start.getMode();
    }
    else if (_create.isActive()) {
        mode = _create.getMode();
    }
    else if (_join.isActive()) {
        mode = _join.getMode();
    }
    else if (_lobby.isActive()) {
        mode = _lobby.getMode();
    }
    else if (_gameplay.isActive()) {
        mode = _gameplay.getMode();
    } else if (_win.isActive()) {
        mode = _win.getMode();
    }
    else {
        mode = constants::GameMode::Start;
    }

    if (_input != nullptr && _input->getEscape()) {
        mode = constants::GameMode::Start;
    }

    if (_mode != mode) {
        // leaving mode
        switch (_mode) {
        case constants::GameMode::None:
            break;
        case constants::GameMode::Loading:
            _loading.dispose();
            break;
        case constants::GameMode::Start:
            _start.dispose();
            break;
        case constants::GameMode::CreateGame:
            _lobby.setRoomID("");
            _create.dispose();
            break;
        case constants::GameMode::JoinGame:
            _lobby.setRoomID(_join.getRoomID());
            _join.dispose();
            break;
        case constants::GameMode::Lobby:
            _lobby.dispose();
            break;
        case constants::GameMode::Game:
            _win.setWinner(_network->getData()->getWinner());
            _gameplay.dispose();
            break;
        case constants::GameMode::Win:
            _win.dispose();
            break;
        }

        // entering mode
        switch (mode) {
        case constants::GameMode::None:
            break;
        case constants::GameMode::Loading:
            break;
        case constants::GameMode::Start:
            _network = make_shared<NetworkController>(); // reset network controller
            _collision = CollisionController::alloc();
            _networkData = make_shared<NetworkData>(); // reset network data
            _network->attachData(_networkData);
            if (_input == nullptr) {
                _input = make_shared<InputController>();
                _input->init(Application::get()->getSafeBounds());
            }
            _start.init(_assets);
            break;
        case constants::GameMode::CreateGame:
            _create.init(_assets);
            break;
        case constants::GameMode::JoinGame:
            _join.init(_assets);
            break;
        case constants::GameMode::Lobby:
            _networkData->setStatus(constants::MatchStatus::Waiting);
            _lobby.setNetwork(_network);
            _lobby.init(_assets);
            break;
        case constants::GameMode::Game:
            _network->startGame();
            _gameplay.setNetwork(_network);
            _gameplay.setInput(_input);
            _gameplay.setCollision(_collision);
            _gameplay.init(_assets);
            break;
        case constants::GameMode::Win:
            _networkData->setStatus(constants::MatchStatus::Ended);
            _win.setNetwork(_network);
            _win.init(_assets);
            break;
        }

        _mode = mode;
    }
    
    if (_input != nullptr) {
        _input->update(timestep);
    }
    
    switch (_mode) {
    case constants::GameMode::Loading:
        _loading.update(0.01f);
        break;
    case constants::GameMode::Start:
        _start.update(timestep);
        break;
    case constants::GameMode::CreateGame:
        _create.update(timestep);
        break;
    case constants::GameMode::JoinGame:
        _join.update(timestep);
        break;
    case constants::GameMode::Lobby:
        _network->update(timestep);
        _lobby.update(timestep);
        break;
    case constants::GameMode::Game:
        if (_input->getReset()) {
            _gameplay.dispose();
            _gameplay.setNetwork(_network);
            _gameplay.setInput(_input);
            _gameplay.setCollision(_collision);
            _gameplay.init(_assets);
        }
        _network->update(timestep);
        _gameplay.update(timestep);
        break;
    case constants::GameMode::Win:
        _network->update(timestep);
        _win.update(timestep);
        break;
    default:
        CULogError("No corresponding mode.");
        break;
    }
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void GhostedApp::draw() {
    switch (_mode) {
    case constants::GameMode::Loading:
        _loading.render(_batch);
        break;
    case constants::GameMode::Start:
        _start.render(_batch);
        break;
    case constants::GameMode::CreateGame:
        _create.render(_batch);
        break;
    case constants::GameMode::JoinGame:
        _join.render(_batch);
        break;
    case constants::GameMode::Lobby:
        _lobby.render(_batch);
        break;
    case constants::GameMode::Win:
        _win.render(_batch);
        break;
    case constants::GameMode::Game:
        _gameplay.draw(_batch, _shaderBatch);
        break;
    }
}

/**
 * Internal helper to build the scene graph.
 *
 * Scene graphs are not required.  You could manage all scenes just like
 * you do in 3152.  However, they greatly simplify scene management, and
 * have become standard in most game engines.
 */
void GhostedApp::buildScene() {
    // Build the scene from these assets

    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());

    _loading.init(_assets);

    // Queue up the other assets
    _assets->loadDirectoryAsync("json/assets.json", nullptr);
    _assets->loadDirectoryAsync("json/start.json", nullptr);
    _assets->loadDirectoryAsync("json/join.json", nullptr);
    _assets->loadDirectoryAsync("json/game.json", nullptr);
    _assets->loadDirectoryAsync("json/lobby.json", nullptr);
    _assets->loadDirectoryAsync("json/win.json", nullptr);
}

/**
 * Internal helper to build the shader.
 *
 * Scene graphs are not required.  You could manage all scenes just like
 * you do in 3152.  However, they greatly simplify scene management, and
 * have become standard in most game engines.
 */
void GhostedApp::buildShader() {
    _shader = Shader::alloc(SHADER(_vsource),SHADER( _fsource));

}
