#include <zap/actor/ActorDonutBlock.h>
#include <zap/Zap.h>
#include <red/util/SpriteUtil.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::ActorDonutBlock, red::ActorDonutBlock)

const ActorCreateInfo zap::ActorDonutBlock::cCreateInfo = {
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

Profile* zap::ActorDonutBlock::sProfile = zap::getRegistrar()->newProfile<zap::ActorDonutBlock>("donut_block")
    .resources<"obj_chikuwa_block">(ProfileInfo::cResType_Course)
    .flag(Profile::cFlag_DrawCullCheck)
    .createInfo(&cCreateInfo)
    .build();

zap::ActorDonutBlock::ActorDonutBlock(const ActorCreateParam& param)
    : red::ActorDonutBlock(param)
{ }
