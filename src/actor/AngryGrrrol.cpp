#include <zap/actor/AngryGrrrol.h>
#include <audio/GameAudio.h>
#include <zap/Zap.h>

/*
    TODO:
    - DRCTouch
    - Sounds (SE_OBJ_TEKKYU_CRASH)
    - Hit wall effect (RP_Enm_Collision_5?)
    - Land effect? (RP_Cmn_LandingSmoke_35)
*/

constexpr f32 cChaseAcceleration = 0.00025f;
constexpr f32 cBaseSpeed = 0.5f;

SEAD_RTTI_OVERRIDE_IMPL(zap::AngryGrrrol, Enemy);

using CC = ActorCollisionCheck;
ActorCollisionCheck::CollisionData zap::AngryGrrrol::cCollisionData = {
    .center_offset = { 0.0f, 0.0f },
    .half_size = { 12.0f, 12.0f },
    .shape_type = CC::cShapeType_Circle,
    .kind = CC::cKind_Enemy,
    .attack = CC::cAttack_Shell,
    .vs_kind = CC::TargetKind(
        CC::cTargetKind_Player |
        CC::cTargetKind_Yoshi |
        CC::cTargetKind_Enemy |
        CC::cTargetKind_Killer |
        CC::cTargetKind_ChibiYoshi
    ),
    .vs_damage = CC::DamageFrom(
        CC::cDamageFrom_FireBall |
        CC::cDamageFrom_IceBall |
        CC::cDamageFrom_Star
    ),
    .status = CC::cStatus_MoveKill,
    .callback = &Enemy::normal_collcheck
};

Profile* zap::AngryGrrrol::cProfile = zap::getRegistrar()->newProfile<zap::AngryGrrrol>("angrygrrrol")
    .resources<"guruguru">(ProfileInfo::cResType_Course)
    .build();

zap::AngryGrrrol::AngryGrrrol(const ActorCreateParam& param)
    : Enemy(param)
    , mModel(nullptr)
    , mEffectSparks()
{ }

ActorBase::Result zap::AngryGrrrol::create() {
    // Model
    mModel = AnimModel::create("guruguru", "guruguru", 0, 0, 1);
    // TODO: Play tex srt anim
    
    // Hitbox
    mCollisionCheck.set(this, AngryGrrrol::cCollisionData);
    reviveCollisionCheck();
    
    // Terrain collision
    static const ActorBgCollisionCheck::Sensor foot = { -8.0f, 8.0f, -16.0f };
    static const ActorBgCollisionCheck::Sensor headwall = { -8.0f, 8.0f, 16.0f };
    mBgCheckObj.set(this, &foot, &headwall, &headwall);
    
    // Flags copied from Grrrol, I don't know what all these do
    using F = ActorBgCollisionCheck::SensorFlag::Bit;
    mBgCheckObj.getSensorFlag(cDirType_Down).setBit(
        F::cBit_43
    );
    mBgCheckObj.getSensorFlag(cDirType_Right).setBit(
        F::cBit_BreakBlocks,
        F::cBit_6, F::cBit_9, F::cBit_10, F::cBit_15, F::cBit_26, F::cBit_27, F::cBit_36, F::cBit_43, F::cBit_54, F::cBit_56
    );
    mBgCheckObj.getSensorFlag(cDirType_Left).setBit(
        F::cBit_BreakBlocks,
        F::cBit_6, F::cBit_9, F::cBit_10, F::cBit_15, F::cBit_26, F::cBit_27, F::cBit_36, F::cBit_43, F::cBit_54, F::cBit_56
    );
    mBgCheckObj.getSensorFlag(cDirType_Up).setBit(
        F::cBit_BreakBlocks,
        F::cBit_6, F::cBit_15, F::cBit_43, F::cBit_54, F::cBit_56
    );
    
    updateModel();
    return cResult_Success;
}

bool zap::AngryGrrrol::execute() {
    // Chase player
    
    sead::Vector2f d2p;
    if (searchNearPlayer(d2p) == -1)
        return true; // No player found
    
    mSpeed.x += d2p.x * cChaseAcceleration; // TODO: Make this configurable
    
    const f32 maximum = cBaseSpeed * 3; // TODO: Make this configurable
    mSpeed.x = sead::Mathf::clamp2(-maximum, mSpeed.x, maximum);

    calcSpeedY_();
    posMove_();
    
    // Terrain collision
    bgCheck_();
    if (bgCheckFoot_()) {
        mSpeed.y = 0;
    }
    
    if (bgCheckWall_()) {
        mSpeed.x = 0;
    }
    
    // Sparks effect
    if (sead::Mathf::abs(mSpeed.x) > 0.5f) {
        sead::Vector3f effectPos(mPos.x, mPos.y - 16.0f, mPos.z + 50.0f);
        mEffectSparks.createEffect(RP_Gorogoro_move_0, &effectPos, nullptr, nullptr);
    }
    
    // Roll sound
    GameAudio::getAudioObjMap()->holdSound("SE_OBJ_TEKKYU_ROLL", mActorUniqueID.getValue(), mPos, 17);
    
    // Rotate model
    mAngle.z() -= sead::Mathf::deg2idx(2.0f * mSpeed.x);
    updateModel();
    
    return true;
}

bool zap::AngryGrrrol::draw() {
    mModel->draw();
    return true;
}

void zap::AngryGrrrol::updateModel() {
    sead::Matrix34f mtx;
    mtx.makeRTIdx(mAngle, mPos);
    mModel->setMtxRT(mtx);
    mModel->setScale(mScale);
    mModel->calcMdl();
}
