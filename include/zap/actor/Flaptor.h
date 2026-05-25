#pragma once

#include <enemy/Enemy.h>
#include <actor/Profile.h>
#include <graphics/JointBlendModel.h>
#include <enemy/EnemyEatData.h>
#include <enemy/EnemyChibiYoshiEatData.h>

namespace zap {

class Flaptor : public Enemy {
    SEAD_RTTI_OVERRIDE(Flaptor, Enemy)

public:
    enum MoveType {
        Stationary = 0,
        Horizontal = 1,
        Vertical   = 2
    };

public:
    static Profile* sProfile;

    Flaptor(const ActorCreateParam& param);
    ~Flaptor() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;
    
    bool createIceActor() override;
    
    void calcMdl_Base() override;
    
    void vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    void vsYoshiHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    
    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData;
    
    DECLARE_STATE_ID(Flaptor, Patrol)
    DECLARE_STATE_ID(Flaptor, Dive)
    DECLARE_STATE_ID(Flaptor, Land)
    
    DECLARE_STATE_VIRTUAL_ID_OVERRIDE(Flaptor, DieOther)

private:
    JointBlendModel* mModel;
    s8 mMoveType;
    f32 mPatrolRange;
    f32 mReturnSpeed;
    sead::Vector3f mStartPos;
    s32 mJointBoneIdx;
    EnemyEatData mYoshiEatData;
    EnemyChibiYoshiEatData mBabyYoshiEatData;
};

} // namespace zap
