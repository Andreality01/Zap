#pragma once

#include <actor/Actor.h>
#include <graphics/AnimModel.h>

namespace zap {

class TimeClock : public Actor {
    SEAD_RTTI_OVERRIDE(TimeClock, Actor)

public:
    static const Profile* cProfile;

    TimeClock(const ActorCreateParam& param);
    ~TimeClock() override = default;
    
    // basic lifecycle funcs:
    Result create() override;
    bool execute() override;
    bool draw() override; 
    
    // custom funcs
    void updateModel();
    void collect();
    
    static const ActorCollisionCheck::CollisionData cCollisionData;
    
private:
    AnimModel* mModel;
};

} // namespace zap
