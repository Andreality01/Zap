#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>
#include <graphics/AnimModel.h>
#include <effect/EffectObj.h>

namespace zap {

class Clef : public Actor {
    SEAD_RTTI_OVERRIDE(Clef, Actor);

public:
    static const Profile* cProfile;

    Clef(const ActorCreateParam& param);    
    ~Clef() override = default;

    Result create() override;
    bool execute() override;
    bool draw() override; 
    
    void updateModel();
    void collect();

    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData;

private:
    AnimModel* mClefModel;
    bool mCollected;
    bool mCollecting; // for anim
    f32 mCollectAnimProgress;
    f32 mBaseYPos;
    f32 mBaseScale;
    f32 mTime;

    EffectObj mEffect1;
    EffectObj mEffect2;
    EffectObj mEffect3;
    EffectObj mEffect4;
};

} // namespace zap
