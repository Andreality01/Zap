#pragma once

#include <enemy/Enemy.h>
#include <graphics/AnimModel.h>
#include <effect/EffectObj.h>

namespace zap {

class AngryGrrrol : public Enemy {
    SEAD_RTTI_OVERRIDE(AngryGrrrol, Enemy);

public:
    static const Profile* cProfile;

    AngryGrrrol(const ActorCreateParam& param);
    ~AngryGrrrol() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;
    
    void updateModel();
    
    static const ActorCollisionCheck::CollisionData cCollisionData;
    
private:
    AnimModel* mModel;
    EffectObj mEffectSparks;
};

} // namespace zap
