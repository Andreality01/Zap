#pragma once

#include <telkin/Print.h>
#include <zap/actor/Clef.h>
#include <actor/ActorState.h>
#include <actor/Actor.h>
#include <actor/Profile.h>
#include <graphics/AnimModel.h>

namespace zap {

class Note : public ActorMultiState {
    SEAD_RTTI_OVERRIDE(Note, ActorMultiState)

public:
    static Profile* sProfile;

    Note(const ActorCreateParam& param);
    ~Note() override = default;

    Result create() override;
    bool execute() override;
    bool draw() override; 
    
    void updateModel();
    void setParent(ActorUniqueID parent) {
        mClefParent = parent;
        tk::println("Hello I am a note and my parent is now set :3");
    }
    void collect();
    void reset();
    
    [[nodiscard]]
    u32 getManagerID() {
        return mManagerID;
    }

    static const ActorCreateInfo cCreateInfo;
    static const ActorCollisionCheck::CollisionData cCollisionData;

    DECLARE_STATE_ID(Note, Idle)
    DECLARE_STATE_ID(Note, Active)
    DECLARE_STATE_ID(Note, Collecting)
    
private:
    AnimModel* mModel;
    
    ActorUniqueID mClefParent;
    
    u32 mManagerID;

    bool mCollected;

    ParentMovementMgr mMovementHandler;
};

} // namespace zap
