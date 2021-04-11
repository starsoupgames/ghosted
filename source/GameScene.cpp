#include "GameScene.h"

using namespace cugl;
using namespace std;

#define CONE_WIDTH 75
#define CONE_LENGTH 200
#define ROOM_SIZE 960

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

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    GameMode::init(assets);

    Size dimen = Application::get()->getDisplaySize();
    dimen *= constants::SCENE_WIDTH / dimen.width;
    Rect bounds = Application::get()->getSafeBounds();

    Application::get()->setClearColor(Color4f::BLACK);

    // Init data models
    _networkData = NetworkData::alloc();
    _network->attachData(_networkData);

    // Init controllers
    _input.init(bounds);
    _collision.init();

    _root = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _root->addChild(_assets->get<scene2::SceneNode>("game"));
    _root->setContentSize(dimen);
    _root->doLayout();
    addChild(_root);


    GameMap _gameMap = GameMap();
    _gameMap.generateRandomMap();
    vector<shared_ptr<Texture>> textures;
    textures.push_back(_assets->get<Texture>("dim_floor_texture"));
    textures.push_back(_assets->get<Texture>("dim_door_texture"));
    _gameMap.setTextures(textures);
    for (auto& node : _gameMap.getNodes()) {
        _root->addChild(node);
    }


    // Initialize the countdown
    _countdown = 0;

    // Initialize ending messages
    _winNode = scene2::Label::alloc("You win!", _assets->get<Font>("gyparody"));
    _winNode->setAnchor(Vec2::ANCHOR_CENTER);
    _winNode->setPosition(dimen.width / 2.0f, dimen.height / 2.0f);
    _winNode->setForeground(Color4::YELLOW);

    _loseNode = scene2::Label::alloc("You lose!", _assets->get<Font>("gyparody"));
    _loseNode->setAnchor(Vec2::ANCHOR_CENTER);
    _loseNode->setPosition(dimen.width / 2.0f, dimen.height / 2.0f);
    _loseNode->setForeground(Color4::YELLOW);

    _palNode = scene2::AnimationNode::alloc(_assets->get<Texture>("pal_texture"), 4, 21);
    _root->addChild(_palNode);
    _palModel = Pal::alloc(Vec2(-100, 0));
    _palModel->setNode(_palNode);
    _palNode->setPriority(constants::Priority::Player);

    _ghostNode = scene2::AnimationNode::alloc(_assets->get<Texture>("ghost_texture"), 4, 21);
    _root->addChild(_ghostNode);
    _ghostModel = Ghost::alloc(Vec2(100, 0));
    _ghostModel->setNode(_ghostNode);
    _ghostNode->setPriority(constants::Priority::Player);

    if (_network->isHost()) {
        _player = _palModel;
        _otherPlayers.push_back(_ghostModel);
    }
    else {
        _player =_ghostModel;
        _otherPlayers.push_back(_palModel);
    }
    _networkData->setPlayer(_player);
    _networkData->setOtherPlayers(_otherPlayers);
    
    vector<Vec2> coneShape;
    Vec2 tl(-CONE_WIDTH, CONE_LENGTH);
    Vec2 tr(CONE_WIDTH, CONE_LENGTH);
    Vec2 bl(-CONE_WIDTH / 8, 0);
    Vec2 br(CONE_WIDTH / 8, 0);

    coneShape.push_back(br);
    coneShape.push_back(bl);
    coneShape.push_back(tl);
    coneShape.push_back(tr);

    _visionNode = scene2::PolygonNode::alloc(coneShape);
    _visionNode->setColor(Color4f(1, 1, 1, .2));
    _visionNode->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
    _visionNode->setPriority(constants::Priority::Player);
    
    _root->addChild(_visionNode);

    _player->getNode()->setAnchor(Vec2::ANCHOR_CENTER);
    for (auto& p : _otherPlayers) {
        p->getNode()->setAnchor(Vec2::ANCHOR_CENTER);
    }
    
    return true;
}

/**
 * Disposes of all(non - static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();

        _input.dispose();
        // _collision.dispose()

        _network = nullptr;
        _networkData = nullptr;

        _root = nullptr;
        _palNode = nullptr;
        _palModel = nullptr;
        _ghostNode = nullptr;
        _ghostModel = nullptr;
        _visionNode = nullptr;

        _active = false;
    }
}

/** Function to sort player node priorities */
bool comparePlayerPriority(const shared_ptr<Player>& p1, const shared_ptr<Player>& p2) {
    return p1->getLoc().y > p2->getLoc().y;
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    Size dimen = Application::get()->getDisplaySize();
    dimen *= constants::SCENE_WIDTH / dimen.width;
    Vec2 center(dimen.width / 2, dimen.height / 2);

    // Process input and update player states
    _input.update(timestep);
//    _slot->update(timestep);

    Vec2 move = _input.getMove();
    Vec2 direction = _input.getDirection();

    _player->setMove(move);
    _player->setIdle(move == Vec2::ZERO);
    if (direction != Vec2::ZERO) {
        _player->setDir(direction);
    }

    bool interact = _input.getInteraction();
    
    shared_ptr<Trap> trap = nullptr;
    
    for (shared_ptr s : _slots) {
        s->update(timestep);
    }
    
    if (_player->getType() == Player::Type::Pal) {
        if (!_palModel->getSpooked()) {
            if (interact) {
                auto slotTexture = _assets->get<Texture>("slot");
                shared_ptr<BatterySlot> slot = BatterySlot::alloc(Vec2(0, 100));
                slot->setTextures(slotTexture, _root->getContentSize());
                slot->getNode()->setScale(0.05f);
                slot->setLoc(_player->getLoc());
                _root->addChild(slot->getNode());
                slot->setCharge();
                slot->getNode()->setColor(Color4f::GREEN);
                _slots.push_back(slot);
//                CULog("ACTIVATE SLOT");
            }
        }
    }
    else {
        if (interact) {
            if (_traps.size() != 0) {
                float distance = ROOM_SIZE/2;
                shared_ptr<Trap> tPtr = nullptr;
                
                for (shared_ptr<Trap> t : _traps) {
                    Vec2 n = t->getLoc() -_player->getLoc();
                    float d = n.length();
                    if (d < distance && !t->getTriggered()) {
                        tPtr = t;
                    }
                }
                trap = tPtr;
            }
            if (trap == nullptr || _traps.size() == 0) {
                auto trapTexture = _assets->get<Texture>("trap");
                shared_ptr<Trap> t = Trap::alloc(Vec2(0, 0));
                t->setTextures(trapTexture, _root->getContentSize());
                t->getNode()->setScale(0.25f);
                _root->addChild(t->getNode());
                _traps.push_back(t);
                trap = t;
            }
            
            if (!trap->getArmed() && !trap->getTriggered()) {
                trap->setArmed(true);
                trap->setLoc(_player->getLoc());
//                CULog("SET TRAP");
            } else if (!trap->getTriggered()) {
                trap->setTriggered(true);
                trap->getNode()->setColor(Color4f::RED);
//                CULog("TRIGGER TRAP");
            }
        }
    }

    _player->update(timestep);
    updateVision(_player);
    for (auto& p : _otherPlayers) {
        p->update(timestep);
        updateVision(p);
    }

    vector<shared_ptr<Player>> allPlayers;
    copy(_otherPlayers.begin(), _otherPlayers.end(), back_inserter(allPlayers));
    allPlayers.push_back(_player);
    sort(allPlayers.begin(), allPlayers.end(), &comparePlayerPriority);
    CUAssertLog(allPlayers.size() < constants::PRIORITY_RANGE, "Number of players exceeds priority range.");
    for (unsigned i = 0; i < allPlayers.size(); ++i) {
        allPlayers[i]->getNode()->setPriority(constants::Priority::Player + 1 + i);
    }

    // Update camera
    _root->setPosition(center - _player->getLoc());

    // Check collisions
    _collision.checkForCollision(_ghostModel, _palModel);
    _collision.checkForCollision(_ghostModel, _visionNode);
    
    if (trap != nullptr && (trap->getTriggered() && trap->getArmed())) {
        _collision.checkForCollision(_palModel, trap);
        trap->setArmed(false);
    }
    
    // TODO: Implement checking battery collisions
    // _collision.checkInBounds(_ghostModel, bounds);    need to define bounds


    // If ghost is tagged, lower tag timer
    int tagTimer = _ghostModel->getTimer();
    if (tagTimer > 0) {
        _ghostModel->setTimer(tagTimer - 1);
    }
    if (_ghostModel->getTimer() == 0) {
        _ghostModel->setTagged(false);
    }

    // Pal case
    if (_network->isHost()) {
        if (_palModel->getSpooked() && _complete == false) {
            // Player loses
            _complete = true;
            _loseNode->setVisible(true);
            _countdown = 120;
        }
        else if (_palModel->getBatteries() == 3 && _complete == false) {
            // Player wins
            _complete = true;
            _winNode->setVisible(true);
            _countdown = 120;
        }

        if (tagTimer == 0) {
            _ghostNode->setVisible(false);
        }
        else {
            _ghostNode->setVisible(true);
        }
    }
    // Ghost case
    else {
        if (_palModel->getSpooked() && _complete == false) {
            // Player wins
            _complete = true;
            _winNode->setVisible(true);
            _countdown = 120;
        }
        else if (_palModel->getBatteries() == 3 && _complete == false) {
            // Player loses
            _complete = true;
            _loseNode->setVisible(true);
            _countdown = 120;
        }
    }

    // Reset the game if a win condition is reached
    if (_countdown > 0) {
        _countdown--;
    }
    else if (_countdown == 0) {
        reset();
    }
}

void GameScene::updateVision(const std::shared_ptr<Player>& player) {
    if (player->getType() != Player::Type::Pal) return;
    
    Vec2 dir = player->getDir();
    // convert degrees to radians
    float angle = atan2(dir.y, dir.x);
    angle = fmod(angle - M_PI_2, 2*M_PI);
    if (angle < 0) angle += 2 * M_PI;

    _visionNode->setAngle(angle);
    _visionNode->setPosition(player->getLoc());
}
