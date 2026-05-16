#pragma once

#include <zap/actor/ActorDonutBlock.h>
#include <actor/Profile.h>

namespace zap {

class FrozenDonutBlock : public zap::ActorDonutBlock {
    SEAD_RTTI_OVERRIDE(FrozenDonutBlock, zap::ActorDonutBlock)

public:
    static Profile* sProfile;
    
    FrozenDonutBlock(const ActorCreateParam& param);
    ~FrozenDonutBlock() override = default;
    
    Result create() override;
    
    void loadActorRes() override;
    
    static const ActorCreateInfo cCreateInfo;
};

}
