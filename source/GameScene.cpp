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
 * For a map with 3x3 rooms, this means that each room should be 14 meters (1m per tile),
 *  this means that out total dimensions for the physics world should be 42m x 42m
 * 
 * -----------------However----------------
 * Right now there's no conversion from pixels to Box2d units, so 1m is effectively 1px
 * For now, each room is 1120px by 1120px. For a 3x3 room, this results in a map of size
 * 3360m x 3360m
 */
#define DEFAULT_WIDTH  42.0f
#define DEFAULT_HEIGHT 42.0f

#define PIXEL_WIDTH    3360.0f
#define PIXEL_HEIGHT   3360.0f
vector<Vec2> BATTERY_SPAWNS = {
    Vec2(0,3), 
    Vec2(3,3), 
    Vec2(6,3), 
    Vec2(0,3), 
    Vec2(3,6), 
    Vec2(6,4), 
    Vec2(0,6), 
    Vec2(3,14), 
    Vec2(6,14)
};


#define SCENE_WIDTH  1024
#define SCENE_HEIGHT 576


Vec2 WALLS[4][2][2][4] = {
    {// north
        {// door
            {Vec2(0.05,14),Vec2(1.05,12),Vec2(6,12),Vec2(6,14)},
            {Vec2(13.95,14),Vec2(8,14),Vec2(8,12),Vec2(12.95,12)}
        },
        {// wall
            {Vec2(0,14),Vec2(1,12),Vec2(7,12),Vec2(7,14)},
            {Vec2(14,14),Vec2(7.01,14),Vec2(7.01,12),Vec2(13,12)}
        }
    },
    {// east
        {// door
            {Vec2(14,14),Vec2(13,12),Vec2(13,7),Vec2(14,7)},
            {Vec2(13,0),Vec2(14,0),Vec2(14,5),Vec2(13,5)}
        },
        {// wall
            {Vec2(14,14),Vec2(13,12),Vec2(13,6.01),Vec2(14,6.01)},
            {Vec2(13,0),Vec2(14,0),Vec2(14,6),Vec2(13,6)}
        }
    },
    {// south
    },
    {// west
        {// door
            {Vec2(0,14),Vec2(0,7),Vec2(1,7),Vec2(1,12)},
            {Vec2(0,0),Vec2(1,0),Vec2(1,5),Vec2(0,5)}
        },
        {// wall
            {Vec2(0,14),Vec2(0,6.01),Vec2(1,6.01),Vec2(1,12)},
            {Vec2(0,0),Vec2(1,0),Vec2(1,6),Vec2(0,6)}
        }
    }
};




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
    if (!GameMode::init(assets, constants::GameMode::Game, "game")) return false;
    if (!GameMode::init(assets, constants::GameMode::Game)) return false;
    
    Size dimen = computeActiveSize();

    // Init the Box2d world
    Rect box2dRect = Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    _world = physics2::ObstacleWorld::alloc(box2dRect, Vec2(0, 0));
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
        _collision->beginContact(contact);
    };
    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        _collision->beforeSolve(contact, oldManifold);
    };
    

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    //_scale = dimen.width == SCENE_WIDTH ? dimen.width / box2dRect.size.width : dimen.height / box2dRect.size.height;
    _scale = PIXEL_WIDTH / DEFAULT_WIDTH;

    _debugNode = scene2::SceneNode::alloc();

    setDebug(true);

    _root = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    //_root->addChild(_assets->get<scene2::SceneNode>("game"));
    _root->setContentSize(dimen);
    _root->doLayout();
    _root->setName("root");
    addChild(_root, 0);

    _dimRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
//    _dimRoot->addChild(_assets->get<scene2::SceneNode>("game"));
    _dimRoot->setContentSize(dimen);
    _dimRoot->doLayout();
    _dimRoot->setName("dim_root");
    _root->addChild(_dimRoot);

    _litRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _litRoot->setContentSize(dimen);
    _litRoot->doLayout();
    _litRoot->setName("lit_root");
    _root->addChild(_litRoot);

    _topRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _topRoot->setContentSize(dimen);
    _topRoot->doLayout();
    _topRoot->setName("top_root");
    _root->addChild(_topRoot);

    _gameMap = GameMap::alloc(_assets, _root);

    // _gameMap->generateBasicMap(0);
    _gameMap->generateRandomMap();
    _collision->setGameMap(_gameMap);


    // Sets the textures of the room nodes and adds them to _root
    // Helper method that goes through all room objects, sets the textures of its nodes, and adds
    // them to root. This same helper will create the box2d objects for the room's walls,
    // obstacles, and batteryslot
    shared_ptr<Texture> floorTexture = _assets->get<Texture>("dim_floor_texture");
    shared_ptr<Texture> litFloorTexture = _assets->get<Texture>("lit_floor_texture");

    // shared_ptr<Texture> doorTexture =_assets->get<Texture>("dim_door_texture");
    //shared_ptr<Texture> litDoorTexture = _assets->get<Texture>("lit_door_texture"); // TODO put this in for loop

    shared_ptr<RoomParser> parser = make_shared<RoomParser>();

    shared_ptr<Texture> slotTexture =_assets->get<Texture>("slot_empty");

    // Player models
    _assets->get<Texture>("pal_doe_texture")->setName("pal_sprite_doe");
    _assets->get<Texture>("pal_seal_texture")->setName("pal_sprite_seal");
    _assets->get<Texture>("pal_tanuki_texture")->setName("pal_sprite_tanuki");
    _assets->get<Texture>("ghost_texture")->setName("ghost_sprite");

    Vec2 spawnOffset = _gameMap->getStartRank() * constants::WALL_DIMENSIONS;

    for (auto& s : { "doe", "seal", "tanuki" }) {
        auto palNode = scene2::AnimationNode::alloc(_assets->get<Texture>("pal_" + string(s) + "_texture"), 6, 24);
        auto hitboxNode = scene2::PolygonNode::alloc(Rect(Vec2 (0, 0), constants::PLAYER_HITBOX_DIMENSIONS));
        hitboxNode->setColor(Color4f::BLUE);
        hitboxNode->setVisible(false);

        auto palShadowNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("pal_shadow_texture"));
        auto palSmokeNode = scene2::AnimationNode::alloc(_assets->get<Texture>("pal_effect_texture"), 2, 19);
        

        Vec2 spawn = spawnOffset;
        if (string(s) == "doe") {
            spawn += Vec2(440, 420);
        }
        else if (string(s) == "seal") {
            spawn += Vec2(680, 420);
        }
        else {
            spawn += Vec2(560, 360);
        }
        auto palModel = Pal::alloc(spawn);
        palModel->setNode(palNode, palShadowNode, palSmokeNode);
        palModel->setHitbox(hitboxNode);
        hitboxNode->setVisible(true);
        _topRoot->addChild(palNode);


        _players.push_back(palModel);
    }

    auto ghostNode = scene2::AnimationNode::alloc(_assets->get<Texture>("ghost_texture"), 4, 24);
    ghostNode->setAnchor(Vec2::ANCHOR_CENTER);
    ghostNode->setPriority(constants::Priority::Player);

    auto ghostShadowNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("ghost_shadow_texture"));
    auto hitboxNode = scene2::PolygonNode::alloc(Rect(Vec2(0, 0), constants::PLAYER_HITBOX_DIMENSIONS));
    hitboxNode->setColor(Color4f::BLUE);
    hitboxNode->setVisible(false);

    

    auto ghostModel = Ghost::alloc(Vec2(560, 560)+(_gameMap->getEndRank()*constants::WALL_DIMENSIONS));
    ghostModel->setNode(ghostNode, ghostShadowNode);
    ghostModel->setHitbox(hitboxNode);
    ghostModel->setTimer(0);
    _litRoot->addChild(ghostNode);
    _players.push_back(ghostModel);

    _network->getData()->setPlayers(_players);
    _players = _network->getData()->getPlayers();

    _network->getData()->setGameMap(_gameMap);

    _gameMap->setPlayer(_network->getData()->getPlayer()->player);
    _gameMap->setPlayers(_network->getData()->getPlayers());
    
    vector<Vec2> coneShape;
    Vec2 tl(-CONE_WIDTH, CONE_LENGTH);
    Vec2 tr(CONE_WIDTH, CONE_LENGTH);
    Vec2 bl(-CONE_WIDTH / 8, 0);
    Vec2 br(CONE_WIDTH / 8, 0);


    coneShape.push_back(br);
    coneShape.push_back(bl);
    coneShape.push_back(tl);
    coneShape.push_back(tr);

    
    // Game UI
//    _gameUI = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
//    _gameUI->setContentSize(dimen);
//    _gameUI->doLayout();
//
//    auto game = _assets->get<scene2::SceneNode>("game");
//    _gameUI->addChild(game);
    
//    _root->addChild(_gameUI, 1);
    
    _litRoot->addChild(_debugNode, 1);

    // Temp background color
    Application::get()->setClearColor(Color4f::BLACK);
    return true;
}

/**
 * Disposes of all(non - static) resources allocated to this mode.
 */
void GameScene::dispose() {
    setActive(false);

    _input = nullptr;
    _collision = nullptr;
    _network = nullptr;
    _gameMap = nullptr;
    _root = nullptr;

    _players.clear();
    _world = nullptr;

    GameMode::dispose();
}

/** Function to sort player node priorities */
bool GameScene::comparePlayerPriority(const shared_ptr<Player>& p1, const shared_ptr<Player>& p2) {
    return p1->getLoc().y > p2->getLoc().y;
}

/**
 * Adds the physics object to the physics world and loosely couples it to the scene graph
 *
 * @param obj    The physics object to add
 * @param node   The scene graph node to attach it to
 * @param zoff   The z-offset for drawing
 * @param hide   True if the node should only be visible in a light source
 */
void GameScene::addObstacle(const shared_ptr<physics2::Obstacle>& obj, const shared_ptr<scene2::SceneNode>& node, float zoff, bool hide) {
    _world->addObstacle(obj);
    obj->setDebugScene(_debugNode);
    
    Vec2 test = obj->getPosition();
    CULog("%f, %f", test.x, test.y);


    // Position the scene graph node (enough for static objects)
    node->setPosition(obj->getPosition() * _scale);
    if (hide) {
        _litRoot->addChild(node, zoff);
    }
    else {
        _topRoot->addChild(node, zoff);
    }
    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        CULog("listener being added");
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs) {
            CULog("Position in callback: ");
            CULog("%f, %f", obj->getPosition().x, obj->getPosition().y);
            weak->setPosition(obs->getPosition() * _scale);
            weak->setAngle(obs->getAngle());
            });
    }
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    Size dimen = computeActiveSize();
    Vec2 center(dimen.width / 2, dimen.height / 2);
    
    auto player = _gameMap->getPlayer();

    // Process movement input and update player states
    _gameMap->move(_input->getMove(), _input->getDirection());
    _gameMap->update(timestep);
    for (auto& p : _players) {
        if (p != nullptr) {
            updateVision(p);
        }
    }
    
//    for (auto& t : _gameMap->getTraps()) {
//        if (t != nullptr) {
//            _audio->setTrapSound(t);
//        }
//    }

    // Checks if the ghost should be revealed, commented out because no
    // tagging yet
    /**
    if (_player->getType() == constants::PlayerType::Pal) {
        _ghostNode->setVisible(_gameMap->getGhost()->getTagged());
    }
    */

    if (_input->getInteraction()) {
        _gameMap->handleInteract();
    }


    // Checks if the ghost should be revealed, commented out because no
    // tagging yet
    /**
    if (_player->getType() == constants::PlayerType::Pal) {
        _ghostNode->setVisible(_gameMap->getGhost()->getTagged());
    }
    */

    // Sets priority of player nodes
    vector<shared_ptr<Player>> sortedPlayers;
    for (auto& p : _players) {
        if (p != nullptr) sortedPlayers.push_back(p);
    }
    sort(sortedPlayers.begin(), sortedPlayers.end(), [this](const shared_ptr<Player>& p1, const shared_ptr<Player>& p2) { return comparePlayerPriority(p1, p2); }) ;
    CUAssertLog(sortedPlayers.size() < constants::PRIORITY_RANGE, "Number of players exceeds priority range.");
    for (unsigned i = 0; i < sortedPlayers.size(); ++i) {
        auto node = sortedPlayers[i]->getNode();
        node->setPriority(constants::Priority::Player + 1 + i);
        auto smoke = node->getChildByName("smoke");
        if (smoke != nullptr) smoke->setPriority(node->getPriority() - 0.01f);
        auto shadow = node->getChildByName("shadow");
        if (shadow != nullptr) shadow->setPriority(node->getPriority() - 0.02f);
    }

    // Update camera
    _litRoot->setPosition(center - player->getLoc());
    _dimRoot->setPosition(center - player->getLoc());
    _topRoot->setPosition(center - player->getLoc());
    


    // Pal wins
    if (_gameMap->getTeleCount() == 0) {
        _network->getData()->setWinner(constants::PlayerType::Pal);
    }

    // Ghost wins
    bool allPalsSpooked = true;
    for (auto& p : _players) {
        if (p->getType() == constants::PlayerType::Pal) {
            allPalsSpooked = allPalsSpooked && dynamic_pointer_cast<Pal>(p)->getSpooked();
        }
    }
    if (allPalsSpooked) {
        _network->getData()->setWinner(constants::PlayerType::Ghost);
    }

    if (_network->getData()->getWinner() != constants::PlayerType::Undefined) {
        _mode = constants::GameMode::Win;
    }
}

void GameScene::updateVision(const std::shared_ptr<Player>& player) {
    if (player->getType() != constants::PlayerType::Pal) return;

    
    // TODO fix all of this
    
    Vec2 dir = player->getDir();
    // convert degrees to radians
    float angle = atan2(dir.y, dir.x);
    angle = fmod(angle - M_PI_2, 2*M_PI);
    if (angle < 0) angle += 2 * M_PI;

}

void GameScene::draw(const std::shared_ptr<SpriteBatch>& batch, const std::shared_ptr<SpriteBatch>& shaderBatch) {
    if (_gameMap->getPlayer()->getType() == constants::PlayerType::Pal) {
        float roomLights[constants::MAX_ROOMS * 3];
        int i = 0;
        for (auto& room : _gameMap->getRooms()) {
            Vec2 slotPos = room->getSlot()->getLoc();
            roomLights[i] = _dimRoot->getPosition().x + slotPos.x;
            roomLights[i + 1] = _dimRoot->getPosition().y + slotPos.y;
            roomLights[i + 2] = room->getLight() ? 1 : 0; // set to 1 if room's light is on, 0 if not
            i += 3;
        }
        //just to fill rest of values if the number of rooms < MAX_ROOMS
        for (int j = i; j < (constants::MAX_ROOMS * 3); ++j) {
            roomLights[j] = 0;
        }

        float palLights[constants::MAX_PALS * 4];
        i = 0;
        for (auto& player : _players) {
            if (player != nullptr && player->getType() == constants::PlayerType::Pal) {
                palLights[i] = _topRoot->getPosition().x + player->getNode()->getPosition().x;
                palLights[i + 1] = _topRoot->getPosition().y + player->getNode()->getPosition().y;
                palLights[i + 2] = 1; // set to 1 if pal is active, 0 if not

                // because vision cone is broken
                Vec2 dir = player->getDir();
                float angle = atan2(dir.y, dir.x);
                angle = fmod(angle - M_PI_2, 2 * M_PI);
                if (angle < 0) angle += 2 * M_PI;
                palLights[i + 3] = (angle - M_PI / 2);

                i += 4;
            }
        }
        // fill rest of values if the number of pals < MAX_PALS
        for (int j = i; j < (constants::MAX_PALS * 4); ++j) {
            palLights[j] = 0;
        }
        batch->begin(_camera->getCombined());
        batch->setBlendFunc(_srcFactor, _dstFactor);
        batch->setBlendEquation(_blendEquation);
        _litRoot->render(batch, _root->getNodeToWorldTransform(), _color);
        batch->end();

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
        _topRoot->render(batch, _root->getNodeToWorldTransform(), _color);
//        _gameUI->render(batch, _root->getNodeToWorldTransform(), _color);
        batch->end();
    }
    else {
        batch->begin(_camera->getCombined());
        batch->setBlendFunc(_srcFactor, _dstFactor);
        batch->setBlendEquation(_blendEquation);
        _litRoot->render(batch, _root->getNodeToWorldTransform(), _color);
        _topRoot->render(batch, _root->getNodeToWorldTransform(), _color);
//        _gameUI->render(batch, _root->getNodeToWorldTransform(), _color);
        batch->end();
    }
}

/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size GameScene::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width / dimen.height;
    float ratio2 = ((float)SCENE_WIDTH) / ((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH / dimen.width;
    }
    else {
        dimen *= SCENE_HEIGHT / dimen.height;
    }
    return dimen;
}

void GameScene::reset() {
    auto assets = _assets;
    dispose();
    init(assets);
}
