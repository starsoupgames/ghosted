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

    // the demos used offset to center the world node in the screen, I don't think we have to use this because
    // we have a camera
    //Vec2 offset((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);

    _debugNode = scene2::SceneNode::alloc();
    //_debugNode->setScale(_scale); // Debug node draws in PHYSICS coordinates

    setDebug(true);

    _root = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    //_root->addChild(_assets->get<scene2::SceneNode>("game"));
    _root->setContentSize(dimen);
    _root->doLayout();
    addChild(_root, 0);

    _dimRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
//    _dimRoot->addChild(_assets->get<scene2::SceneNode>("game"));
    _dimRoot->setContentSize(dimen);
    _dimRoot->doLayout();
    _root->addChild(_dimRoot);

    _litRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _litRoot->setContentSize(dimen);
    _litRoot->doLayout();
    
    _root->addChild(_litRoot);

    _topRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _topRoot->setContentSize(dimen);
    _topRoot->doLayout();
    _root->addChild(_topRoot);

    _gameMap = GameMap::alloc(_assets, _litRoot);

    _gameMap->generateBasicMap(0);
    _collision->setGameMap(_gameMap);

    // Sets the textures of the room nodes and adds them to _root
    // Helper method that goes through all room objects, sets the textures of its nodes, and adds
    // them to root. This same helper will create the box2d objects for the room's walls,
    // obstacles, and batteryslot
    shared_ptr<Texture> floorTexture = _assets->get<Texture>("dim_floor_texture");
    shared_ptr<Texture> litFloorTexture = _assets->get<Texture>("lit_floor_texture");

    // shared_ptr<Texture> doorTexture =_assets->get<Texture>("dim_door_texture");
    //shared_ptr<Texture> litDoorTexture = _assets->get<Texture>("lit_door_texture"); // TODO put this in for loop

    shared_ptr<Texture> slotTexture =_assets->get<Texture>("slot");
    for (auto& room : _gameMap->getRooms()) {
        string roomCode = room->getDoorsStr();
        shared_ptr<Texture> doorTexture = _assets->get<Texture>(roomCode);
        shared_ptr<scene2::PolygonNode> node = scene2::PolygonNode::allocWithTexture(doorTexture);

        shared_ptr<scene2::PolygonNode> floorNode = scene2::PolygonNode::allocWithTexture(floorTexture);
        //offset for transformation matrix
        Mat4 floorOffset = Mat4::createTranslation(Vec3(31.25, 0, 0));
        //scale for transformation matrix
        Mat4 floorScale = Mat4::createScale(Vec2(2.56, 2.56));
        //multiply them all together to use them all
        floorOffset = floorOffset.multiply(floorScale);
        //floorNode->setScale(Vec2(5, 5));
        floorNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
        floorNode->setAlternateTransform(floorOffset);
        floorNode->chooseAlternateTransform(true);
        node->addChild(floorNode);


        node->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
        room->setNode(node);
        room->initContents(slotTexture, _root->getContentSize());
        _gameMap->addSlot(room->getSlot());
        _dimRoot->addChild(node);
        node->setPosition(room->getOrigin());

        // Create obstacles for walls
        shared_ptr<scene2::PolygonNode> sprite;
        int count = 0;
        int wallRef;
        Vec2 ranking = room->getRanking();
        int xOffset = ranking.x * 14;
        int yOffset = ranking.y * 14;
        vector<Vec2> vertices;
        SimpleTriangulator triangulator;
        shared_ptr<physics2::PolygonObstacle> wallobj;

        // Loop through doors of the room
        for (auto check : room->getDoors()) {
            // Figure out if this should be a door or a wall
            wallRef = check ? 0 : 1;
            // Loop through the two obstacles inside the door list
            if (count != 2) {
                for (auto& obs : WALLS[count][wallRef]) {
                    vertices.clear();
                    for (auto& pos : obs) {
                        vertices.push_back(Vec2(pos.x + xOffset, pos.y + yOffset));
                    }
                    Poly2 wall = Poly2(vertices);

                    triangulator.set(wall);
                    triangulator.calculate();
                    wall = triangulator.getPolygon();
                    wall.setGeometry(Geometry::SOLID);

                    wallobj = physics2::PolygonObstacle::alloc(wall);
                    wallobj->setName("Wall" + strtool::to_string(count) + strtool::to_string(wallRef));
                    wallobj->setBodyType(b2_staticBody);
                    wallobj->setDensity(0.0f);
                    wallobj->setFriction(0);
                    wallobj->setRestitution(0.1f);
                    wallobj->setDebugColor(Color4::YELLOW);

                    sprite = scene2::PolygonNode::alloc();
                    CULog("Adding wall: ");
                    //addObstacle(wallobj, sprite, 1, false);
                }
            }
            count += 1;
        
        }

        //scene graph for lit versions of entities, or entities only visible under light
        shared_ptr<scene2::PolygonNode> litNode = scene2::PolygonNode::allocWithTexture(litFloorTexture);
        litNode->setAlternateTransform(floorOffset);
        litNode->chooseAlternateTransform(true);
        //litNode->addChild(scene2::PolygonNode::allocWithTexture(litDoorTexture));
        litNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
        _litRoot->addChild(litNode);
        litNode->setPosition(room->getOrigin());
    }

    // Player models
    _assets->get<Texture>("pal_doe_texture")->setName("pal_sprite_doe");
    _assets->get<Texture>("pal_seal_texture")->setName("pal_sprite_seal");
    _assets->get<Texture>("pal_tanuki_texture")->setName("pal_sprite_tanuki");
    _assets->get<Texture>("ghost_texture")->setName("ghost_sprite");

    for (auto& s : { "doe", "seal", "tanuki" }) {
        auto palNode = scene2::AnimationNode::alloc(_assets->get<Texture>("pal_" + string(s) + "_texture"), 4, 24);

        auto palShadowNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("pal_shadow_texture"));
        auto palSmokeNode = scene2::AnimationNode::alloc(_assets->get<Texture>("pal_smoke_texture"), 1, 19);
        
        Vec2 spawn;
        if (string(s) == "doe") {
            spawn = Vec2(3, 2);
        }
        else if (string(s) == "seal") {
            spawn = Vec2(11, 2);
        }
        else {
            spawn = Vec2(3, 10);
        }
        auto palModel = Pal::alloc(spawn);
        palModel->setNode(palNode, palShadowNode, palSmokeNode);
        _topRoot->addChild(palNode);

        _players.push_back(palModel);
    }

    auto ghostNode = scene2::AnimationNode::alloc(_assets->get<Texture>("ghost_texture"), 4, 24);
    ghostNode->setAnchor(Vec2::ANCHOR_CENTER);
    ghostNode->setPriority(constants::Priority::Player);

    auto ghostShadowNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("ghost_shadow_texture"));

    auto ghostModel = Ghost::alloc(Vec2(11, 10));
    ghostModel->setNode(ghostNode, ghostShadowNode);
    ghostModel->setTimer(0);
    _litRoot->addChild(ghostNode);
    _players.push_back(ghostModel);

    _network->getData()->setPlayers(_players);
    _players = _network->getData()->getPlayers();

    _gameMap->setPlayer(_network->getData()->getPlayer()->player);
    
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
    _visionNode->setAnchor(Vec2::ANCHOR_CENTER);
    _visionNode->setPriority(constants::Priority::Player);
    
    _root->addChild(_visionNode);
    
    // Joystick UI
    _leftnode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("dpad_left"));
    _leftnode->SceneNode::setAnchor(cugl::Vec2::ANCHOR_MIDDLE_RIGHT);
    _leftnode->setScale(0.35f);
    _leftnode->setVisible(false);
    _topRoot->addChild(_leftnode);

    _rightnode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("dpad_right"));
    _rightnode->SceneNode::setAnchor(cugl::Vec2::ANCHOR_MIDDLE_LEFT);
    _rightnode->setScale(0.35f);
    _rightnode->setVisible(false);
    
    _topRoot->addChild(_rightnode);
    
//    _gameUI = _assets->get<scene2::SceneNode>("game");
//    _gameMap->getPlayer()->getNode()->addChild(_gameUI);

    
    _litRoot->addChild(_debugNode, 1);

    // Temp background color
    Application::get()->setClearColor(Color4f::GRAY);
    return true;
}

/**
 * Disposes of all(non - static) resources allocated to this mode.
 */
void GameScene::dispose() {
    GameMode::dispose();
    setActive(false);

    _input = nullptr;
    _collision = nullptr;
    _network = nullptr;
    _gameMap = nullptr;
    _root = nullptr;
    _visionNode = nullptr;
    _leftnode = nullptr;
    _rightnode = nullptr;

    _players.clear();
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
        
    // FLOATING JOYSTICK UI
//    if (_input->withJoystick()) {
//        if (_input->getMove() != Vec2::ZERO) {
//            CULog("SET LEFT");
//            _leftnode->setVisible(true);
//        } else {
//            _leftnode->setVisible(false);
//        }
//
//        if (_input->getDirection() != Vec2::ZERO) {
//            _rightnode->setVisible(true);
//        } else {
//            _rightnode->setVisible(false);
//        }
//        auto player = _gameMap->getPlayer();
//
//        _leftnode->setPosition(_input->getLJoystick() - player->getLoc());
//        utils::Log(_input->getLJoystick()  - _players[0]->getLoc());
//        utils::Log(player->getLoc());
//        _rightnode->setPosition(_input->getRJoystick());
//    } else {
//        _leftnode->setVisible(false);
//        _rightnode->setVisible(false);
//    }

    // Checks if the ghost should be revealed, commented out because no
    // tagging yet
    /**
    if (_player->getType() == constants::PlayerType::Pal) {
        _ghostNode->setVisible(_gameMap->getGhost()->getTagged());
    }
    */

    // Delete after batteries implemented, will be handled in gamemap
    if (player->getType() == constants::PlayerType::Pal) {
        if (!dynamic_pointer_cast<Pal>(player)->getSpooked()) {
            if (_input->getInteraction()) {
                auto slotTexture = _assets->get<Texture>("slot");
                shared_ptr<BatterySlot> slot = BatterySlot::alloc(Vec2(0, 100));
                slot->setTextures(slotTexture, _root->getContentSize());
                slot->getNode()->setScale(0.05f);
                _root->addChild(slot->getNode());
                slot->getNode()->setVisible(true);
                slot->setLoc(player->getLoc());
                slot->setCharge();
                slot->getNode()->setColor(Color4f::GREEN);
                _gameMap->addSlot(slot);
                _gameMap->handleInteract();
            }
        }
    }
    else {
        if (_input->getInteraction()) {
            _gameMap->handleInteract();
        }
    }

    // Checks if the ghost should be revealed, commented out because no
    // tagging yet
    /**
    if (_player->getType() == constants::PlayerType::Pal) {
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
    _root->setPosition(center - player->getLoc());
    
    // TODO: TEMPORARY WIN CODE
    /*
    Vec2 winOrigin = _gameMap->getWinRoomOrigin() + (constants::ROOM_DIMENSIONS / 2);
    for (auto& p : _players) {
        if ((p->getLoc() - winOrigin).length() < 100) {
            _network->getData()->setWinner(p->getType());
            break;
        }
    }
    */
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

    _visionNode->setAngle(angle);
    _visionNode->setPosition(player->getLoc());
}

void GameScene::draw(const std::shared_ptr<SpriteBatch>& batch, const std::shared_ptr<SpriteBatch>& shaderBatch) {
    if (_gameMap->getPlayer()->getType() == constants::PlayerType::Pal) {
        float roomLights[constants::MAX_ROOMS * 3];
        int i = 0;
        for (auto& room : _gameMap->getRooms()) {
            roomLights[i] = _root->getPosition().x + room->getOrigin().x + constants::ROOM_CENTER.x;
            roomLights[i + 1] = _root->getPosition().y + room->getOrigin().y + constants::ROOM_CENTER.x;
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
                palLights[i] = _root->getPosition().x + player->getLoc().x;
                palLights[i + 1] = _root->getPosition().y + player->getLoc().y;
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
        batch->end();
    }
    else {
        batch->begin(_camera->getCombined());
        batch->setBlendFunc(_srcFactor, _dstFactor);
        batch->setBlendEquation(_blendEquation);
        _litRoot->render(batch, _root->getNodeToWorldTransform(), _color);
        _topRoot->render(batch, _root->getNodeToWorldTransform(), _color);
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
