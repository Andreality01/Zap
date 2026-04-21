#pragma once

#include <enemy/Enemy.h>
#include <graphics/JointBlendModel.h>
#include <parent/ParentMovementMgr.h>
#include <actor/Profile.h>

namespace zap {

class Biddybud : public Enemy {
    SEAD_RTTI_OVERRIDE(Biddybud, Enemy)

public:
    static const Profile* cProfile;

    Biddybud(const ActorCreateParam& param);
    ~Biddybud() override = default;

    Result create() override;
    bool execute() override;
    bool draw() override;

    bool createIceActor() override;

    void vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    void vsYoshiHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    
    void updateModel();

    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData;

    DECLARE_STATE_ID(Biddybud, Idle)

    DECLARE_STATE_VIRTUAL_ID_OVERRIDE(Biddybud, DieOther)

private:
    JointBlendModel* mModel;
    ParentMovementMgr mMovementHandler;
};

} // namespace zap
