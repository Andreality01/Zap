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
    static Profile* sProfile;

    Cataquack(const ActorCreateParam& param);
    ~Cataquack() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;

    void calcMdl_Base() override;

    void vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    void vsYoshiHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    void vsEnemyHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;

    bool hitCallback_Fire(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    bool hitCallback_Ice(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    bool hitCallback_Shell(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    
    void handleMovement();
    void setChaseMode(bool active);
    // From KuriboBase
    void landonEffect();
    // From KuriboBase
    bool checkGround();
    
    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData; // collision info
    
    DECLARE_STATE_ID(Cataquack, Walk);
    DECLARE_STATE_ID(Cataquack, Turn);
    DECLARE_STATE_ID(Cataquack, Launch);
    DECLARE_STATE_VIRTUAL_ID_OVERRIDE(Cataquack, DieFall)

private:
    JointBlendModel* mModel;

    // Debug only
    char mImGuiWindowID[24];

    bool mHasLanded; // From KuriboBase
    bool mForceLanded; // From KuriboBase
    bool mChasing;
    ActorUniqueID mTarget; // Stores the PlayerBase actor being chased
    sead::Vector2f mLaunchForce; // In NSMBU speed vector units, exponential
};

} // namespace zap
