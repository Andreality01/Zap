#pragma once

#include <enemy/Enemy.h>
#include <actor/Profile.h>
#include <graphics/JointBlendModel.h>
#include <map_obj/ParentMovementMgr.h>
#include <enemy/EnemyEatData.h>
#include <enemy/EnemyChibiYoshiEatData.h>

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
    void calcMdl_Base() override;

    void vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    void vsYoshiHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;

    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData;

    DECLARE_STATE_ID(Biddybud, Idle)

    DECLARE_STATE_VIRTUAL_ID_OVERRIDE(Biddybud, DieOther)

private:
    JointBlendModel* mModel;
    EnemyEatData mYoshiEatData;
    EnemyChibiYoshiEatData mChibiYoshiEatData;
    ParentMovementMgr mMovementHandler;
};

} // namespace zap
