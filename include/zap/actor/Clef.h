#pragma once

#include <actor/Profile.h>
#include <effect/EffectObj.h>
#include <actor/ActorState.h>
#include <graphics/AnimModel.h>
#include <map_obj/ParentMovementMgr.h>

namespace zap {

class Clef : public ActorMultiState {
    SEAD_RTTI_OVERRIDE(Clef, ActorMultiState);

public:
    static Profile* sProfile;

    Clef(const ActorCreateParam& param);    
    ~Clef() override = default;

    Result create() override;
    bool execute() override;
    bool draw() override; 
    
    void scanNotes();
    
    void updateModel();
    void collect();

    void noteCollected(); // todo maybe add params here
    
    [[nodiscard]]
    bool isReady() { return mNotes != nullptr; }

    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData;

private:
    AnimModel* mClefModel;
    bool mCollected;
    bool mCollecting; // for anim
    f32 mCollectAnimProgress;
    f32 mBaseScale;
    f32 mTime;
    u32 mManagerID;
    u32 mRewardID;

    u32 mGameStartEventID;
    u32 mGameWonEventID;
    
    u32 mTargetNoteCount;
    u32 mFoundNoteCount;
    u32 mStoredNoteCount;
    u32 mCollectedNoteCount;
    u32 mScanAttempt;
    
    ActorUniqueID* mNotes;

    EffectObj mEffect1;
    EffectObj mEffect2;
    EffectObj mEffect3;
    EffectObj mEffect4;

    ParentMovementMgr mMovementHandler;
};

} // namespace zap
