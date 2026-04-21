#include <zap/actor/Biddybud.h>
#include <zap/Zap.h>
#include <red/util/SpriteUtil.h>
#include <effect/EffectCreateUtil.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::Biddybud, Enemy)

CREATE_STATE_ID(zap::Biddybud, Idle)

CREATE_STATE_VIRTUAL_ID_OVERRIDE(zap::Biddybud, Enemy, DieOther)

// Configuration
static constexpr f32 cScaleFactor = 0.17f; // 3DW models are large
static constexpr f32 cInvScaleFactor = 1.0f / cScaleFactor;

const ActorCreateInfo zap::Biddybud::cCreateInfo = {
    .offset_x = 8, .offset_y = -8,
    .spawn_range = {
        .offset_x = 0, .offset_y = 0,
        .half_size_x = 8, .half_size_y = 8
    },
    .cull_range = { 
        .up = 0, .down = 0, .left = 0, .right = 0
    },
    .flag = 0
};

using CC = ActorCollisionCheck;
const CC::CollisionData zap::Biddybud::cCollisionData = {
    .center_offset = { 0.0f, 0.0f },
    .half_size = { 8.0f, 8.0f },
    .shape_type = CC::ActorCollisionCheck::cShapeType_Box,
    .kind = CC::cKind_Enemy,
    .attack = CC::cAttack_None,
    .vs_kind = CC::TargetKind( // Copied from Kuribo
        CC::cTargetKind_Player |
        CC::cTargetKind_Enemy |
        CC::cTargetKind_Item |
        CC::cTargetKind_Tama |
        CC::cTargetKind_ChibiYoshi |
        CC::cTargetKind_Unk10 |
        CC::cTargetKind_DrcTouch
    ),
    .vs_damage = CC::cDamageFrom_All,
    .status = CC::cStatus_None,
    .callback = &Enemy::normal_collcheck
};

const Profile* zap::Biddybud::cProfile = zap::getRegistrar()->newProfile<zap::Biddybud>("biddybud")
    .resources<"tenten_w">(ProfileInfo::cResType_Course)
    .createInfo(&cCreateInfo)
    .build();

zap::Biddybud::Biddybud(const ActorCreateParam& param)
    : Enemy(param)
    , mModel(nullptr)
    , mYoshiEatData(mActorUniqueID)
    , mChibiYoshiEatData(mActorUniqueID)
    , mMovementHandler()
{ }

ActorBase::Result zap::Biddybud::create() {
    mModel = JointBlendModel::create("tenten_w", "tenten_w", 3, 1, 1);
    mScale = sead::Vector3f(cScaleFactor, cScaleFactor, cScaleFactor);
    mModel->playTexAnim("bud");
    mModel->playTexSrtAnim("FlyWait");
    mModel->playSklAnim("FlyWait");
    
    // Setting: Colour
    mModel->getTexAnim(0)->getFrameCtrl().setFrame(red::SpriteUtil::getNybble5(this));
    mModel->getTexAnim(0)->getFrameCtrl().setRate(0.0f);
    
    // Setup hitbox
    mCollisionCheck.set(this, cCollisionData);
    reviveCollisionCheck();
    
    // Movement setup
    const u8 nybble20 = red::SpriteUtil::getNybble20(this);
    if (nybble20 > cMovement_MushroomList) {
        tk::fatal("Biddybud movement type was out of bounds");
    }
    const ParentMovementType movementType = static_cast<ParentMovementType>(nybble20);
    u32 movementMask = mMovementHandler.getMaskForMovementType(movementType);
    mMovementHandler.link(mPos, movementMask, mParamEx.course.movement_id); // nybble 21-22
    
    // Yoshi eat ability
    mEatDataPtr = &mYoshiEatData;
    mYoshiEatData.setEatType(EatData::cEatType_Drink);
    
    // Baby Yoshi eat ability
    mChibiYoshiEatDataPtr = &mChibiYoshiEatData;
    mChibiYoshiEatData.setEatType(ChibiYoshiEatData::cEatType_Drink);

    changeState(StateID_Idle);

    updateModel();
    
    return cResult_Success;
}

bool zap::Biddybud::execute() {
    // Movement
    if (isState(StateID_Idle)) {
        mMovementHandler.execute();
        mPos = mMovementHandler.getPosition();
        //mAngle.z() = mMovementHandler.getAngle();
    }

    // Delete when offscreen
    screenOutCheck(0);
    
    executeState();

    updateModel();
    
    // TODO: Track skeletal anim?
    
    return true; 
}

bool zap::Biddybud::draw() {
    mModel->draw();
    return true;
}

void zap::Biddybud::updateModel() {
    mModel->update(mPos + sead::Vector3f(0.0f, -8.0f, 0.0f), mAngle, mScale, !isState(StateID_Ice));
}

bool zap::Biddybud::createIceActor() {
    sead::Vector3f pos = {
        mPos.x,
        mPos.y - 8.0f,
        mPos.z
    };
    
    IceInfo info = { 
        IceInfo::makeParam(cIceType_Square),
        pos,
        mScale * cInvScaleFactor * 1.5f,
        nullptr
    };
    return mIceMgr.createIce(info);
}

void zap::Biddybud::vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) {
    Actor* other = cc_other->getOwner();
    
    switch (fumiCheck(cc_self, cc_other, cFumiSeType_Normal)) {
        case cFumiType_Fumi: {
            return setDeathInfo_FumiOther(other, mSpeed); // changes state to DieOther
        }
        
        case cFumiType_MameFumi: {
            return;
        }
        
        case cFumiType_SpinFumi: {
            return setDeathInfo_SpinFumi(other); // changes state to DieFall
        }
        
        default: {
            return Enemy::vsPlayerHitCheck_Normal(cc_self, cc_other);
        }
    }
}

void zap::Biddybud::vsYoshiHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) {
    Actor* other = cc_other->getOwner();
    
    switch (fumiCheck(cc_self, cc_other, cFumiSeType_Normal)) {
        case cFumiType_Fumi: {
            return setDeathInfo_YoshiFumi(other); // changes state to DieYoshiFumi
        }
        
        default: {
            return Enemy::vsYoshiHitCheck_Normal(cc_self, cc_other);
        }
    }
}

/** STATE: Idle */

void zap::Biddybud::initializeState_Idle() {
    
}

void zap::Biddybud::executeState_Idle() {
    
}

void zap::Biddybud::finalizeState_Idle() { }

/** STATE: DieOther */

void zap::Biddybud::initializeState_DieOther() {
    mModel->setAnm("BlowDown", 0.0f, FrameCtrl::cMode_NoRepeat);
    removeCollisionCheck();
}

void zap::Biddybud::executeState_DieOther() {
    // Wait for squish to finish before deleting
    if (mModel->getCurSklAnim()->getFrameCtrl().isStop()) {
        deleteRequest();
        removeCollisionCheck();
        
        // Spawn a puff effect
        // TODO: Change this effect to the correct one from Yoshi stomp?
        EffectCreateUtil::createEffect(RP_Cmn_EnemyBurst_00, &mPos);
    }
}

void zap::Biddybud::finalizeState_DieOther() { }
