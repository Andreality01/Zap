#include <zap/actor/TimeClock.h>
#include <zap/Zap.h>
#include <audio/GameAudio.h>
#include <effect/EffectCreateUtil.h>
#include <game/CourseTimer.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::TimeClock, Actor);

// Register it
Profile* zap::TimeClock::cProfile = zap::getRegistrar()->newProfile<zap::TimeClock>("timeclock")
    .resources<"timeclock">(ProfileInfo::cResType_Course)
    .build();

// Hitbox data
using CC = ActorCollisionCheck;
ActorCollisionCheck::CollisionData zap::TimeClock::cCollisionData = {
    .center_offset = { 0.0f, 0.0f },
    .half_size = { 12.0f, 12.0f },
    .shape_type = CC::cShapeType_Box,
    .kind = CC::cKind_Item,
    .attack = CC::cAttack_None,
    .vs_kind = CC::TargetKind(
        CC::cTargetKind_Player |
        CC::cTargetKind_Item
    ),
    .vs_damage = CC::DamageFrom(
        CC::cDamageFrom_Slip |
        CC::cDamageFrom_HipAttack |
        CC::cDamageFrom_Shell |
        CC::cDamageFrom_PenguinSlip |
        CC::cDamageFrom_Spin |
        CC::cDamageFrom_SpinFall
    ),
    .status = CC::cStatus_None,
    .callback = [](ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) { 
        TimeClock* self = cc_self->getOwner<TimeClock>();
        if (self != nullptr)
            self->collect();
    }
};
    
// Main code
zap::TimeClock::TimeClock(const ActorCreateParam& param)
    : Actor(param)
    , mModel(nullptr)
{ }

ActorBase::Result zap::TimeClock::create() {
    // load it first
    // szs name, then model name inside
    mModel = AnimModel::create("timeclock", "timeclockA");
    // make sure it appears on the first frame
    updateModel();

    // make a hitbox
    mCollisionCheck.set(this, cCollisionData);
    reviveCollisionCheck();
    
    return cResult_Success;
}

bool zap::TimeClock::execute() {
    mAngle.y() -= sead::Mathf::deg2idx(5.0f); // spin 5 degrees per frame
    updateModel();

    return true;
}

bool zap::TimeClock::draw() {
    mModel->draw();
    return true;
}

void zap::TimeClock::updateModel() {
    // just copy our transform info
    sead::Matrix34f mtx;
    mtx.makeRTIdx(mAngle, mPos);
    mModel->setMtxRT(mtx);
    mModel->setScale(mScale);
    mModel->calcMdl();
}

void zap::TimeClock::collect() {
    // spawn puff effect
    sead::Vector3f effectPos = mPos + sead::Vector3f(0.0f, -12.0f, 0.0f);
    EffectCreateUtil::createEffect(RP_FlagPass_1, &effectPos);
    
    // play a sound effect
    GameAudio::getAudioObjMap()->startSound("SE_SYS_CONTINUE_DONE", mPos);
    
    // add the time
    u16 timeDelta = mParam0 & 0xFFF; // Nybbles 10-12
    CourseTimer::instance()->addTimeLimitSeconds(timeDelta);
    
    // bye!
    deleteRequest();
    removeCollisionCheck();
}
