#pragma once

#include <zap/actor/ActorDonutBlock.h>
#include <actor/Profile.h>

namespace zap {

class AntigravityDonutBlock : public zap::ActorDonutBlock {
    SEAD_RTTI_OVERRIDE(AntigravityDonutBlock, zap::ActorDonutBlock)

public:
    static Profile* sProfile;
    
    AntigravityDonutBlock(const ActorCreateParam& param);
    ~AntigravityDonutBlock() override = default;
    
    Result create() override;
    
    DECLARE_STATE_VIRTUAL_ID_OVERRIDE(AntigravityDonutBlock, Falling)
    
    static const ActorCreateInfo cCreateInfo;

private:
    f32 mLiftSpeed;
};

} // namespace zap
