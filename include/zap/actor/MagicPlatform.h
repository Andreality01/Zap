#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>
#include <map_obj/ParentMovementMgr.h>
#include <collision/ActorBoxBgCollision.h>
#include <collision/ActorPolylineBgCollision.h>

namespace zap {

class MagicPlatform : public Actor {
    SEAD_RTTI_OVERRIDE(MagicPlatform, Actor)

public:
    enum CollisionType {
        cCollisionType_Solid,
        cCollisionType_Semisolid,
        cCollisionType_None,
        
        cCollisionType_Max
    };

public:
    static Profile* sProfile;

    MagicPlatform(const ActorCreateParam& param);
    ~MagicPlatform() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;
    
    static const ActorCreateInfo cCreateInfo;
    
private:
    u16* mTileData;
    sead::Vector2u mTileSize;
    ParentMovementMgr mMovementMgr;
    u8 mCollisionType;
    ActorBoxBgCollision mSolidCollider;
    ActorPolylineBgCollision<1> mSemisolidCollider;
};

} // namespace zap
