#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>
#include <graphics/AnimModel.h>

namespace zap {

class Note : public Actor {
    SEAD_RTTI_OVERRIDE(Note, Actor);

public:
    static const Profile* cProfile;

    Note(const ActorCreateParam& param);
    ~Note() override = default;

    Result create() override;
    bool execute() override;
    bool draw() override; 
    
    void updateModel();
    void collect();

    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData;

private:
    AnimModel* mModel;
};

} // namespace zap
