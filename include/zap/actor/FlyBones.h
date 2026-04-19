#pragma once

#include <enemy/Enemy.h>
#include <actor/Profile.h>
#include <graphics/AnimModel.h>

namespace zap {

class FlyBones : public Enemy {
    SEAD_RTTI_OVERRIDE(FlyBones, Enemy)

public:
    static const Profile* cProfile;

    FlyBones(const ActorCreateParam& param);
    ~FlyBones() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;
    
    bool createIceActor() override;
    
    void vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    void vsYoshiHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
    
    void updateModel();
    void loseWings();

    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData;
    
    DECLARE_STATE_ID(FlyBones, Idle)

private:
    AnimModel* mBodyModel;
    AnimModel* mWingsModel;
};

} // namespace zap
