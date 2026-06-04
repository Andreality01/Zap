#pragma once

#include <actor/Actor.h>

namespace zap {

class ActorSpawnerSimple : public Actor {
    SEAD_RTTI_OVERRIDE(ActorSpawnerSimple, Actor);

public:
    static Profile* sProfile;

    ActorSpawnerSimple(const ActorCreateParam& param);
    ~ActorSpawnerSimple() override = default;

    Result create() override;
    bool execute() override;

private:
    u16 mSpawnProfileID;
    u16 mSpawnEventID;
    bool mSpawned;
    bool mMultiUse;
    bool mPrevFrameEvent;
};

} // namespace zap
