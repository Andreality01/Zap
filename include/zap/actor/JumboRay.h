#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>
#include <graphics/AnimModel.h>
#include <collision/ActorPolylineBgCollision.h>

namespace zap {
    
class JumboRay : public Actor {
    SEAD_RTTI_OVERRIDE(JumboRay, Actor)
    
public:
    static Profile* sProfile;
    
    JumboRay(const ActorCreateParam& param);
    ~JumboRay() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;
    
    void updateModel();
    
    f32 waveFunction(f32 x) const;
    f32 waveDerivative(f32 x) const;
    
    static const ActorCreateInfo cCreateInfo;
    
private:
    AnimModel* mModel;
    f32 mTargetAltitude;
    f32 mBaseY;
    f32 mTime;
    DirType mVerticalDirection;
    u32 mArcDuration;
    ActorPolylineBgCollision<2> mCollider;
    u32 mPlayerRideFlags;
};
    
}
