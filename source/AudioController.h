#pragma once

#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__
#include <cugl/cugl.h>
#include "Constants.h"
#include "GameEntities/Trap.h"

#define ARM_SOUND   "arm"
#define TRIGGER_SOUND   "trigger"
#define MENU_MUSIC  "menu"

using namespace std;
using namespace cugl;

class AudioController {
private:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    string _trapSound;
    
    string _menuMusic;
    
    bool _mute;
    
public:
    AudioController() : _menuMusic(""), _trapSound(""), _mute(false) {};
    ~AudioController() { dispose(); };
    
    bool init(std::shared_ptr<cugl::AssetManager>& assets);
    
    string getTrapSound() {
        return _trapSound;
    };
    
    void setTrapSound(shared_ptr<Trap> t);
    
    void setMenuMusic(bool play);
    
    void setMute(bool mute) {
        _mute = mute;
    }
    
    void dispose();
    
    void update(float timestep);
};

#endif /** __AUDIO_CONTROLLER_H__ */
