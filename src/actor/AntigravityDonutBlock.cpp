#include <zap/actor/AntigravityDonutBlock.h>

#include <zap/Zap.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::AntigravityDonutBlock, zap::ActorDonutBlock)

CREATE_STATE_VIRTUAL_ID_OVERRIDE(zap::AntigravityDonutBlock, ChikuwaBlockBase, Falling)

const ActorCreateInfo zap::AntigravityDonutBlock::cCreateInfo = {
    .offset_x = 8, .offset_y = -16,
    .spawn_range = {
        .offset_x = 0, .offset_y = 0,
        .half_size_x = 8, .half_size_y = 8
    },
    .cull_range = {
        .up = 0, .down = 0, .left = 0, .right = 0
    },
    .flag = 0
};

Profile* zap::AntigravityDonutBlock::sProfile = zap::getRegistrar()->newProfile<zap::AntigravityDonutBlock>("antigravity_donut_block")
    .resources<"obj_chikuwa_block">(ProfileInfo::cResType_Course)
    .flag(Profile::cFlag_DrawCullCheck)
    .createInfo(&cCreateInfo)
    .build();

zap::AntigravityDonutBlock::AntigravityDonutBlock(const ActorCreateParam& param)
    : zap::ActorDonutBlock(param)
    , mLiftSpeed(0.0f)
{ }

ActorBase::Result zap::AntigravityDonutBlock::create() {
    red::ActorDonutBlock::create();
    
    mLiftSpeed = 1.0f;
    
    return cResult_Success;
}

/** STATE: Falling */

void zap::AntigravityDonutBlock::initializeState_Falling() {
    mSpeedMax.y = mLiftSpeed;
}

void zap::AntigravityDonutBlock::executeState_Falling() {
    red::ActorDonutBlock::executeState_Falling();
}

void zap::AntigravityDonutBlock::finalizeState_Falling() {
    red::ActorDonutBlock::finalizeState_Falling();
}
