#include "AudioController.h"

using namespace cugl;
using namespace std;

bool AudioController::init(std::shared_ptr<cugl::AssetManager>& assets) {
    _assets = assets;
    return true;
}

void AudioController::dispose() {
//    _trapSound = nullptr;
}

void AudioController::setMenuMusic(bool play) {
    if (play) {
        _menuMusic = MENU_MUSIC;
    } else {
        _menuMusic = "";
    }
}

void AudioController::setTrapSound(shared_ptr<Trap> t) {
    if (t->getArmed() && !t->getTriggered() && !t->doneArming()) {
        if (t->justArmed()) _trapSound = ARM_SOUND;
        else {
            _trapSound = "";
        }
    }
    else if (t->getTriggered() && t->doneArming() && !t->doneTriggering()) {
        if (t->justTriggered()) {
            _trapSound = TRIGGER_SOUND;
        }
        else {
            _trapSound = "";
        }
    }
    else {_trapSound = "";};
}

void AudioController::update(float timestep) {
    if (AudioEngine::get()->getMusicQueue()->getState() == AudioEngine::State::PLAYING && _menuMusic.size() > 0) {
        if (_mute && AudioEngine::get()->getMusicQueue()->getVolume() > 0.0f) {
            AudioEngine::get()->getMusicQueue()->setVolume(0.0f);
        } else if (!_mute && AudioEngine::get()->getMusicQueue()->getVolume() <= 0.0f) {
            AudioEngine::get()->getMusicQueue()->setVolume(1.0f);
        }
    }
    
    if (!(AudioEngine::get()->getMusicQueue()->getState() == AudioEngine::State::PLAYING)
 && _menuMusic.size() > 0) {
        auto musicsource = _assets->get<Sound>(MENU_MUSIC);
        AudioEngine::get()->getMusicQueue()->play(musicsource, true);
//        AudioEngine::get()->play(_menuMusic,musicsource,true,musicsource->getVolume());
    } else if (_menuMusic.size() <= 0) {
        if (AudioEngine::get()->isActive(MENU_MUSIC)) {
            AudioEngine::get()->clear(MENU_MUSIC);
        }
    }
////    CULog("%s",_trapSound.c_str());
//    if (!AudioEngine::get()->isActive(_trapSound) && _trapSound.size() > 0) {
//        auto source = _assets->get<Sound>(_trapSound);
//        AudioEngine::get()->play(_trapSound,source,false,source->getVolume());
//    }
}

