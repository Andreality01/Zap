#pragma once

#include <actor/ActorState.h>
#include <actor/Profile.h>
#include <graphics/AnimModel.h>
#include <effect/EffectObj.h>
#include <map_obj/ParentMovementMgr.h>

namespace zap {

class TimeClock : public ActorState {
    SEAD_RTTI_OVERRIDE(TimeClock, ActorState)

public:
    static Profile* sProfile;

    TimeClock(const ActorCreateParam& param);
    ~TimeClock() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override; 

    void updateModel() const;
    void collect();
    
    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData;
    
private:
    AnimModel* mModel;
    ParentMovementMgr mMovementHandler;

    u32 mReactivationEvent;
    u32 mCollectionEvent;

    f32 mTime; // maybe use for turning anim?
    f32 mCollectAnimProgress;

    EffectObj mCollectEffect;

    u16 mTimeSelectionDelta;

    DECLARE_STATE_ID(TimeClock, Active)
    DECLARE_STATE_ID(TimeClock, Idle)
    DECLARE_STATE_ID(TimeClock, Collecting)
};

} // namespace zap
