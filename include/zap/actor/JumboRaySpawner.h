#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>
#include <map/SwitchFlagMgr.h>

namespace zap {

class JumboRaySpawner : public Actor {
    SEAD_RTTI_OVERRIDE(JumboRaySpawner, Actor)

public:
    static Profile* sProfile;
    
    JumboRaySpawner(const ActorCreateParam& param);
    ~JumboRaySpawner() override = default;
    
    Result create() override;
    bool execute() override;
    
    [[nodiscard]]
    bool isActive() const {
        if (mTriggeringEventID == 0)
            return true;
        
        return SwitchFlagMgr::instance()->isActivated(mTriggeringEventID - 1);
    }
    
    void spawnRay();
    
    static const ActorCreateInfo cCreateInfo;

private:
    u32 mTimer;
    u32 mInterval;
    u32 mLimit;
    u32 mSpawnedCount;
    u32 mTriggeringEventID;
};

}
