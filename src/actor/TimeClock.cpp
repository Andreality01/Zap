#include <zap/actor/TimeClock.h>
#include <zap/Zap.h>
#include <audio/GameAudio.h>
#include <effect/EffectCreateUtil.h>
#include <game/CourseTimer.h>
#include <red/util/SpriteUtil.h>
#include <map/SwitchFlagMgr.h>
#include <game/CourseTask.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::TimeClock, ActorState);

CREATE_STATE_ID(zap::TimeClock, Active)
CREATE_STATE_ID(zap::TimeClock, Collecting)
CREATE_STATE_ID(zap::TimeClock, Idle)

static constexpr f32 cCollectAnimDuration = 9.0f; // frames
static constexpr f32 cCollectAnimTiles = 1.5f;

static constexpr sead::SafeArray<u32, 6> cTimes = {10, 1, 5, 30, 50, 100};

// Register it
Profile* zap::TimeClock::sProfile = zap::getRegistrar()->newProfile<zap::TimeClock>("timeclock")
    .resources<"timeclock">(ProfileInfo::cResType_Course)
    .build();

// Hitbox data
using CC = ActorCollisionCheck;
const ActorCollisionCheck::CollisionData zap::TimeClock::cCollisionData = {
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
        if (TimeClock* self = cc_self->getOwner<TimeClock>(); self != nullptr)
            self->collect();
    }
};

    
// Main code
zap::TimeClock::TimeClock(const ActorCreateParam& param)
    : ActorState(param)
    , mModel(nullptr)
{ }

ActorBase::Result zap::TimeClock::create() {
    // Model
    mModel = AnimModel::create("timeclock", "timeclockA"); // if (mTimeDelta < 30) blue;
    updateModel(); // make sure it appears on the first frame

    // Hitbox
    mCollisionCheck.set(this, cCollisionData);

    // Time selection
    mTimeDelta = cTimes[mParam0 & 0xFFF];
    
    // small clock
    if (mTimeDelta == 1) {
        mCollisionCheck.setHalfSize(6.0f, 6.0f);
        mScale = {0.5f, 0.5f, 0.5f};
    }
    
    // Movement setup
    const u8 nybble20 = red::SpriteUtil::getNybble20(this);
    if (nybble20 > cPos_KinokoLift) {
        tk::fatal("Movement type was out of bounds");
    }
    const ParentMovementType movementType = static_cast<ParentMovementType>(nybble20);
    u32 movementMask = mMovementHandler.getTypeMask(movementType);
    mMovementHandler.link(mPos, movementMask, mParamEx.course.movement_id); // nybble 21-22

    // Event IDs
    mReactivationEvent = (red::SpriteUtil::getNybble5(this) << 4) | red::SpriteUtil::getNybble6(this);
    mCollectionEvent = (red::SpriteUtil::getNybble7(this) << 4) | red::SpriteUtil::getNybble8(this);

    changeState(StateID_Active);

    return cResult_Success;
}

bool zap::TimeClock::execute() {
    // Delete when offscreen
    screenOutCheck(0);
    
    mAngle.y() -= sead::Mathf::deg2idx(5.0f); // spin 5 degrees per frame
    updateModel();

    return true;
}

bool zap::TimeClock::draw() {
    mModel->draw();
    return true;
}

void zap::TimeClock::updateModel() const {
    mModel->update(mPos, mAngle, mScale);
}

void zap::TimeClock::collect() {
    if (isState(StateID_Active)) {
        tk::println("Active and collecting!");
        SwitchFlagMgr::instance()->set(mCollectionEvent, 0, true);

        removeCollisionCheck();
        
        changeState(StateID_Collecting);
    }
    return;
    tk::println("Notactive and notcollecting!");
}

/** STATE: Collecting */
void zap::TimeClock::initializeState_Active() { 
    tk::println(":3 Active!");
    reviveCollisionCheck();
}

void zap::TimeClock::executeState_Active() { 
    mAngle.y() -= sead::Mathf::deg2idx(5.0f); // spin 5 degrees per frame
}

void zap::TimeClock::finalizeState_Active() { }

/** STATE: Collecting */
void zap::TimeClock::initializeState_Collecting() { 
    tk::println(":3 Collecting");

    // Sound
    GameAudio::getAudioObjMap()->startSound("SE_SYS_CONTINUE_DONE", mPos);

    bool useBonusAnim = red::SpriteUtil::getNybble1(this);
    
    if (useBonusAnim) {
        tk::println("Using bonus anim");
        CourseTimer::instance()->setBonusTime(mTimeDelta);
        CourseTask::instance()->doBonusTime(0); // TODO: Set player ID properly
    } else {
        // add the time
        CourseTimer::instance()->addTimeLimitSeconds(mTimeDelta);
    }
    tk::println("HERE");

    bool useCollectAnim = red::SpriteUtil::getNybble2(this);
    if (!useCollectAnim) { // Don't use collect animation
        tk::println("Skipping collect animation");
        // spawn puff effect in place (no following)
        const sead::Vector3f effectPos = mPos + sead::Vector3f(0.0f, -12.0f, 0.0f);
        EffectCreateUtil::createEffect(RP_FlagPass_1, &effectPos);

        // switch directly to the collected state (model doesnt need to change at all)
        changeState(StateID_Idle);
    } // use collect animation: continue to collecting state execute; plays anim
}

void zap::TimeClock::executeState_Collecting() { 
    tk::println("Collecting");
    if (mCollectAnimProgress >= sead::Mathf::pi()) { 
        changeState(StateID_Idle);
    }
    
    f32 yOffset;

    mCollectAnimProgress += 1.0f / cCollectAnimDuration;
    yOffset = sead::Mathf::sin(mCollectAnimProgress) * (cCollectAnimTiles * 16.0f);

    mPos.y += yOffset;

    mAngle.y() += sead::Mathf::deg2idx(30.0f);

    mCollectEffect.createEffect(RP_FlagPass_1, &mPos, nullptr);
}

void zap::TimeClock::finalizeState_Collecting() { }

/** STATE: Idle */
void zap::TimeClock::initializeState_Idle() {
    tk::println(":3 Idle");
}

void zap::TimeClock::executeState_Idle() { 
    // check if re-enabled
    // if (SwitchFlagMgr::instance()->isActivated(mReactivationEvent)) {
    //     changeState(StateID_Active);
    // }
}

void zap::TimeClock::finalizeState_Idle() { }


/** TODO
 * Nybble for "time addition animation" ✅
 * Movement controller 🔄 test
 * Event activation (SwitchFlagMgr) 🔄 test
 * Collect anim ✅
 * Nybble for "collect animation" ✅
 * NSLU time add fix ✅ ty lumi
 * Small clock 
 * Respawning (based on event activation) ❗️ figure this out lol 
 * Negative time / bad clock
 * spin model faster or slower based on time (or add this to a fake timeclock)
 * Add an alternative "simple" animation, similar to points text ui or maybe sparkles at the timer ui
 */


/**
 1. Time dropdown 1, 5, 10, 30, 50, 100 ✅
 2. 50+ uses green 🔄 waiting on mdl
 3. small cock ✅ check offset tho:
 4. Negative time (bad clock?) -> "subtract time" option
*/
