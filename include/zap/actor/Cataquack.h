#pragma once

#include <enemy/Enemy.h>
#include <graphics/JointBlendModel.h>
#include <effect/EffectObj.h>
#include <collision/ActorBoxBgCollision.h>
#include <actor/Profile.h>

namespace zap {

class Cataquack : public Enemy {
    SEAD_RTTI_OVERRIDE(Cataquack, Enemy)

public:
    static Profile* cProfile;

    Cataquack(const ActorCreateParam& param);
    ~Cataquack() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;

    void calcMdl_Base() override;

    void handleMovement();
    void setChaseMode(bool active);
    void controlLaunchHeight();
    
    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData; // collision info
    
    DECLARE_STATE_ID(Cataquack, Walk);
    DECLARE_STATE_ID(Cataquack, Turn);
    DECLARE_STATE_ID(Cataquack, Launch);

private:
    //ActorBoxBgCollision mCollider;
    JointBlendModel* mModel;

    bool mChasing;
    ActorUniqueID mTarget; // stores the PlayerBase actor being chased
    f32 mTargetInitialY;
    f32 mLaunchHeight;
};

} // namespace zap
