#include "GameScene.h"

using namespace cugl;
using namespace std;


/** Length and width of the vision cone in pixels */
#define CONE_WIDTH 75
#define CONE_LENGTH 200
/** Length and width of a room in pixels */
#define ROOM_SIZE 960

/** 
 * Length and Width of the game world in Box2d units (meters).
 * For a map with 3x3 rooms, this means that each room should be ten meters,
 * as we are leaving 5 spare meters of space at the perimeter.
 */
#define DEFAULT_DIMENS  40.0f


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
    if (!GameMode::init(assets, constants::GameMode::Game)) return false;
    
    Size dimen = Application::get()->getDisplaySize();
    dimen *= constants::SCENE_WIDTH / dimen.width;
    Rect bounds = Application::get()->getSafeBounds();
    // Background color
    Application::get()->setClearColor(Color4f::BLACK);

    // Init data models
//    _networkData = NetworkData::alloc();
//    _network->attachData(_networkData);

    // Init the Box2d world
    _world = physics2::ObstacleWorld::alloc(Rect(0, 0, DEFAULT_DIMENS, DEFAULT_DIMENS), Vec2(0, 0));
    _world->activateCollisionCallbacks(true);
    /**
    _world->onBeginContact = [this](b2Contact* contact) {
        _collision->beginContact(contact);
    };
    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        _collision->beforeSolve(contact, oldManifold);
    };
    */


    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    /*
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;
    Vec2 offset((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);
    */
    _gameMap = GameMap::alloc();
    _gameMap->generateRandomMap();
    _palModel = _gameMap->getPal();
    _ghostModel = _gameMap->getGhost();


    _root = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    //_root->addChild(_assets->get<scene2::SceneNode>("game"));
    _root->setContentSize(dimen);
    _root->doLayout();
    addChild(_root);

    _dimRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _dimRoot->addChild(_assets->get<scene2::SceneNode>("game"));
    _dimRoot->setContentSize(dimen);
    _dimRoot->doLayout();
    _root->addChild(_dimRoot);

    _litRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _litRoot->setContentSize(dimen);
    _litRoot->doLayout();
    _root->addChild(_litRoot);

    // Sets the textures of the room nodes and adds them to _root
    // Helper method that goes through all room objects, sets the textures of its nodes, and adds
    // them to root. This same helper will create the box2d objects for the room's walls,
    // obstacles, and batteryslot
    shared_ptr<Texture> floorTexture = _assets->get<Texture>("dim_floor_texture");
    shared_ptr<Texture> doorTexture =_assets->get<Texture>("dim_door_texture");
    shared_ptr<Texture> trapTexture =_assets->get<Texture>("trap");
    shared_ptr<Texture> slotTexture =_assets->get<Texture>("slot");
    for (auto& room : _gameMap->getRooms()) {
        shared_ptr<scene2::PolygonNode> node = scene2::PolygonNode::allocWithTexture(floorTexture);
        node->addChild(scene2::PolygonNode::allocWithTexture(doorTexture));
        node->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
        room->setNode(node);
        room->initContents(trapTexture, slotTexture, _root->getContentSize());
        _gameMap->addTrap(room->getTrap());
        _gameMap->addSlot(room->getSlot());
        _dimRoot->addChild(node);
        node->setPosition(room->getOrigin());
    }

    // Initialize ending messages
    _ghostWinNode = scene2::Label::alloc("The ghost wins!", _assets->get<Font>("gyparody"));
    _ghostWinNode->setAnchor(Vec2::ANCHOR_CENTER);
    _ghostWinNode->setPosition(dimen.width / 2.0f, dimen.height / 2.0f);
    _ghostWinNode->setForeground(Color4::YELLOW);
    _ghostWinNode->setVisible(false);
    _root->addChild(_ghostWinNode);

    _palWinNode = scene2::Label::alloc("The pals win!", _assets->get<Font>("gyparody"));
    _palWinNode->setAnchor(Vec2::ANCHOR_CENTER);
    _palWinNode->setPosition(dimen.width / 2.0f, dimen.height / 2.0f);
    _palWinNode->setForeground(Color4::YELLOW);
    _palWinNode->setVisible(false);
    _root->addChild(_palWinNode);

    _assets->get<Texture>("pal_doe_texture")->setName("pal_sprite");
    _palShadowNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("pal_shadow_texture"));
    _palNode = scene2::AnimationNode::alloc(_assets->get<Texture>("pal_doe_texture"), 3, 24);
    _palModel->setNode(_palNode, _palShadowNode);
    _palNode->setPriority(constants::Priority::Player);
    _palShadowNode->setPriority(_palNode->getPriority() - 1);
    _litRoot->addChild(_palNode);
    _palNode->addChild(_palShadowNode);

    //TODO: THESE NEXT TWO LINES MUST BE REPLACED WITH THE NEW GHOST SPRITE TEXTURE INSTEAD OF THE SEAL
    _assets->get<Texture>("ghost_texture")->setName("ghost_sprite");
    _ghostShadowNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("ghost_shadow_texture"));
    _ghostNode = scene2::AnimationNode::alloc(_assets->get<Texture>("ghost_texture"), 3, 24);
    _ghostModel->setNode(_ghostNode, _ghostShadowNode);
    _ghostModel->setTimer(0);
    _ghostNode->setPriority(constants::Priority::Player);
    _ghostShadowNode->setPriority(_ghostNode->getPriority() - 1);
    _dimRoot->addChild(_ghostNode);
    _ghostNode->addChild(_ghostShadowNode);

    if (_network->isHost()) {
        _player = _palModel;
        _gameMap->setPlayer(_palModel);
        _gameMap->setOtherPlayers({ _ghostModel });
    }
    else {
        CULog("we are the ghost");
        _player = _ghostModel;
        _gameMap->setPlayer(_ghostModel);
        _gameMap->setOtherPlayers({ _palModel });
    }
    _players = vector<shared_ptr<Player>>(4, nullptr);
    _players[0] = _palModel;
    _players[1] = _ghostModel;
    _network->setPlayers(_players);
    
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

    for (auto& p : _players) {
        if (p != nullptr) {
            p->getNode()->setAnchor(Vec2::ANCHOR_CENTER);
        }
    }
    
    return true;
}

/**
 * Disposes of all(non - static) resources allocated to this mode.
 */
void GameScene::dispose() {
    GameMode::dispose();
    setActive(false);

//    _input->dispose();
    _collision = nullptr;
    _network = nullptr;
    _gameMap = nullptr;
    _root = nullptr;
    _palNode = nullptr;
    _ghostNode = nullptr;
    _visionNode = nullptr;

    _player = nullptr;
    _palModel = nullptr;
    _ghostModel = nullptr;
    _players.clear();
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

    shared_ptr<Player> _player = _gameMap->getPlayer();
    vector<shared_ptr<Player>> _otherPlayers = _gameMap->getOtherPlayers();

    // Process movement input and update player states

    // Delete if input works V
    //_input->update(timestep);

    _gameMap->move(_input->getMove(), _input->getDirection());
    // Updates vision cones
    for (auto& p : _players) {
        if (p != nullptr) {
            if (p != _player) { // TODO converge player and otherPlayers in gameMap
                p->update(timestep);
            }
            updateVision(p);
        }
    }

    // Interaction checking should happen here
    
    if (_input->getInteraction()) {
        if (_player->getType() == Player::Type::Pal) {
            if (!(_gameMap->getPal()->getSpooked())) {
                _gameMap->handleInteract(true);
            }
        }
        else {
            if (_gameMap->getTraps().size() != 0) {
                bool ifTrapped = "the below comment";
                // use collisioncontroller to check if pal is on trap
                if (ifTrapped) {
                    _gameMap->handleInteract(false);
                }
            }
        }
    }

    // Updates the models
    _gameMap->update(timestep);

    // Checks if the ghost should be revealed, commented out because no
    // tagging yet
    /**
    if (_player->getType() == Player::Type::Pal) {
        _ghostNode->setVisible(_gameMap->getGhost()->getTagged());
    }
    */

    // Check collisions
    // between players and bounds
    // between pals and walls
    // between ghost and vision cones
    // between ghost and pal
    // between trap and pal if trap is triggered
    // between pal and battery

    // Sets priority of player nodes
    vector<shared_ptr<Player>> sortedPlayers;
    for (auto& p : _players) {
        if (p != nullptr) sortedPlayers.push_back(p);
    }
    sort(sortedPlayers.begin(), sortedPlayers.end(), &comparePlayerPriority);
    CUAssertLog(sortedPlayers.size() < constants::PRIORITY_RANGE, "Number of players exceeds priority range.");
    for (unsigned i = 0; i < sortedPlayers.size(); ++i) {
        sortedPlayers[i]->getNode()->setPriority(constants::Priority::Player + 1 + i);
    }

    // Update camera
    _root->setPosition(center - _player->getLoc());

    // Check if a player has won
    auto complete = _gameMap->getComplete();
    if (complete[0]) {
        // Pals win
        _palWinNode->setVisible(complete[1]);
        // Ghost wins
        _ghostWinNode->setVisible(!complete[1]);
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
    _visionNode->setPosition(player->getLoc() + constants::FLASHLIGHT_OFFSET);
}

void GameScene::draw(const std::shared_ptr<SpriteBatch>& batch, const std::shared_ptr<SpriteBatch>& shaderBatch) {
    if (_network->isHost()) {
        float roomLights[constants::MAX_ROOMS * 3];
        int i = 0;
        for (auto& room : _gameMap->getRooms()) {
            roomLights[i] = _root->getPosition().x + room->getOrigin().x + constants::ROOM_CENTER.x;
            roomLights[i+1] = _root->getPosition().y + room->getOrigin().y + constants::ROOM_CENTER.x;
            // set to 1 if room's light is on, 0 if not
            if (room->getLight()) {
                roomLights[i + 2] = 1;
            } else {
                roomLights[i + 2] = 0;
            }
            i = i + 3;
        }
        //just to fill rest of values if the number of rooms < MAX_ROOMS
        for (int j = i; j < (constants::MAX_ROOMS * 3); ++j) {
            roomLights[j] = 0;
        }

        float palLights[constants::MAX_PALS * 4];
        i = 0;
        for (auto& p : _players) {
            if (p != nullptr && p->getType() == Player::Type::Pal) {
                palLights[i] = _root->getPosition().x + p->getLoc().x + constants::FLASHLIGHT_OFFSET.x;
                palLights[i + 1] = _root->getPosition().y + p->getLoc().y + constants::FLASHLIGHT_OFFSET.y;
                // set to 1 if pal is active 0 if not
                palLights[i + 2] = 1.0;
                palLights[i + 3] = (_visionNode->getAngle() - M_PI / 2);
//                CULog("%f", palLights[i + 3]);
                i = i + 4;
            }
        }
        //just to fill rest of values if the number of pals < MAX_PALS
        for (int j = i; j < (constants::MAX_PALS * 4); ++j) {
            palLights[j] = 0;
        }

        shaderBatch->begin(_camera->getCombined());
        GLint uPlayers = shaderBatch->getShader()->getUniformLocation("uPlayers");
        shaderBatch->getShader()->setUniform4fv(uPlayers, constants::MAX_PALS, palLights);

        GLint uRoomLights = shaderBatch->getShader()->getUniformLocation("uRoomLights");
        shaderBatch->getShader()->setUniform3fv(uRoomLights, constants::MAX_ROOMS, roomLights);

        shaderBatch->setBlendFunc(_srcFactor, _dstFactor);

        shaderBatch->setBlendEquation(_blendEquation);
        _dimRoot->render(shaderBatch, _root->getNodeToWorldTransform(), _color);

        shaderBatch->end();

        batch->begin(_camera->getCombined());
        batch->setBlendFunc(_srcFactor, _dstFactor);
        batch->setBlendEquation(_blendEquation);
        _litRoot->render(batch, _root->getNodeToWorldTransform(), _color);
        batch->end();
    }
    else {
        batch->begin(_camera->getCombined());
        batch->setBlendFunc(_srcFactor, _dstFactor);
        batch->setBlendEquation(_blendEquation);
        _dimRoot->render(batch, _root->getNodeToWorldTransform(), _color);
        _litRoot->render(batch, _root->getNodeToWorldTransform(), _color);
        batch->end();
    }
}


void GameScene::reset() {
    auto assets = _assets;
    dispose();
    init(assets);
}
