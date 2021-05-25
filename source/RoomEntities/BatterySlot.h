#pragma once
/** 
This BatterySlot class contains information about the BatterySlot's texture and level of charge
*/

#include <cugl/cugl.h>
#include "../RoomEntity.h"
#include "../Constants.h"
using namespace std;
using namespace cugl;

class BatterySlot : public RoomEntity {
private:
    shared_ptr<scene2::PolygonNode> _nodeOn;
    shared_ptr<scene2::PolygonNode> _nodeOff;

    /** Level of charge remaining */
    float _charge;

    bool _activate;

public:
    void activate() {
        _activate = true;
    }

    bool activated() {
        return _activate;
    }

    /** Returns the amount of charge the battery has */
    float getCharge() const {
        return _charge;
    }
    
    /** Returns the amount of charge the battery has */
    void setCharge() {
        _charge = constants::SLOT_CHARGE;
        _activate = false;
    }

    /** Updates the battery slot's charge */
    void update(float timestep);

    /** Creates a BatterySlot with the default values */
    BatterySlot() : RoomEntity(), _charge(0), _activate(false) {}

    /** Releases all resources allocated with this Battery Slot */
    ~BatterySlot() { dispose(); }
    
    void setNode(const shared_ptr<scene2::PolygonNode>& nodeOn, const shared_ptr<scene2::PolygonNode>& nodeOff);
    
    /**
    * @return a newly allocated Trap at the origin.
    */
    static shared_ptr<BatterySlot> alloc() {
        shared_ptr<BatterySlot> result = make_shared<BatterySlot>();
        return (result->init() ? result : nullptr);
    }

    static shared_ptr<BatterySlot> alloc(const Vec2& pos) {
        shared_ptr<BatterySlot> result = make_shared<BatterySlot>();
        return (result->init(pos) ? result : nullptr);
    };
    
    void dispose();
    
    void reset();
};
