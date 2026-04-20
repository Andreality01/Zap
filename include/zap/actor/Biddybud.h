#pragma once

#include <enemy/Enemy.h>
#include <graphics/JointBlendModel.h>

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

    void updateModel();

    static const ActorCollisionCheck::CollisionData cCollisionData;

private:
    JointBlendModel* mModel;
};

} // namespace zap
