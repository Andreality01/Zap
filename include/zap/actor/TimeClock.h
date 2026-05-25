#pragma once

#include <actor/Actor.h>
#include <graphics/AnimModel.h>

namespace zap {

class TimeClock : public Actor {
    SEAD_RTTI_OVERRIDE(TimeClock, Actor)

public:
    static Profile* sProfile;

    TimeClock(const ActorCreateParam& param);
    ~TimeClock() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override; 

    void updateModel() const;
    void collect();
    
    static const ActorCollisionCheck::CollisionData cCollisionData;
    
private:
    AnimModel* mModel;
};

} // namespace zap
