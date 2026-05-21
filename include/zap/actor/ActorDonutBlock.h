#pragma once

#include <red/actor/ActorDonutBlock.h>
#include <actor/Profile.h>

namespace zap {

class ActorDonutBlock : public red::ActorDonutBlock {
    SEAD_RTTI_OVERRIDE(ActorDonutBlock, red::ActorDonutBlock)

public:
    static Profile* sProfile;
    
    ActorDonutBlock(const ActorCreateParam& param);
    ~ActorDonutBlock() override = default;
    
    Result create() override;
    
    void loadActorRes() override;
    
    static const ActorCreateInfo cCreateInfo;
    
protected:
    u8 mLength;
};

}
