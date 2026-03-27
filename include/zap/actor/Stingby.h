#pragma once

#include <enemy/Enemy.h>
#include <enemy/EnemyEatData.h>
#include <enemy/EnemyChibiYoshiEatData.h>
#include <graphics/JointBlendModel.h>

namespace zap {

class Stingby : public Enemy {
    SEAD_RTTI_OVERRIDE(Stingby, Enemy)

public:
    static Profile* cProfile;

    Stingby(const ActorCreateParam& param);
    ~Stingby() override = default;

    Result create() override;
    bool execute() override;
    bool draw() override;

    bool createIceActor() override;
    void setIceAnm() override;
    
    void vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    void vsYoshiHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    
    void updateModel();

    static ActorCollisionCheck::CollisionData cCollisionData;

    DECLARE_STATE_ID(Stingby, Idle)
    DECLARE_STATE_ID(Stingby, Notice)
    DECLARE_STATE_ID(Stingby, Chase)
    DECLARE_STATE_ID(Stingby, Return)

    DECLARE_STATE_VIRTUAL_ID_OVERRIDE(Stingby, DieOther)

private:
    JointBlendModel* mModel;
    s32 mJointBoneIdx;
    EnemyEatData mYoshiEatData;
    EnemyChibiYoshiEatData mChibiYoshiEatData;
    const sead::Vector3f mSpawnpoint;

    // Chase state
    f32 mChaseSpeed;
    f32 mChaseVelX;
    f32 mWeavePhase;
    f32 mWeaveOffset;

    // Bobbing state
    f32 mBobPhase;

    // Idle (patrol) state
    f32 mPatrolPhase;
    f32 mPatrolSpeed;

    // Idle pause quirk
    s32 mIdlePauseTimer;

    // Aggro puff quirk
    s32 mAggroPuffTimer;
};

}
