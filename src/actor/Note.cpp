#include <zap/Zap.h>
#include <telkin/Print.h>
#include <zap/actor/Note.h>
#include <actor/ActorMgr.h>
#include <red/util/SpriteUtil.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::Note, ActorMultiState)

CREATE_STATE_ID(zap::Note, Idle)
CREATE_STATE_ID(zap::Note, Active)
CREATE_STATE_ID(zap::Note, Collecting)

static constexpr f32 cScaleFactor = 0.17f;

const ActorCreateInfo zap::Note::cCreateInfo = {
    .offset_x = 8, .offset_y = -8,
    .spawn_range = {
        .offset_x = 0, .offset_y = 0,
        .half_size_x = 8, .half_size_y = 8
    },
    .cull_range = { 
        .up = 0, .down = 0, .left = 0, .right = 0
    },
    .flag = ActorCreateInfo::cFlag_MapObj
};

using CC = ActorCollisionCheck;
const CC::CollisionData zap::Note::cCollisionData = {
    .center_offset = { 0.0f, 0.0f },
    .half_size = { 8.0f, 8.0f },
    .shape_type = CC::ActorCollisionCheck::cShapeType_Box,
    .kind = CC::cKind_Enemy,
    .attack = CC::cAttack_None,
    .vs_kind = CC::TargetKind(
        CC::cTargetKind_Player
    ),
    .vs_damage = CC::cDamageFrom_All,
    .status = CC::cStatus_None,
    .callback = [](ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) { 
        zap::Note* self = cc_self->getOwner<zap::Note>();
        if (self != nullptr) {
            self->collect();
        }
    }
};

Profile* zap::Note::sProfile = zap::getRegistrar()->newProfile<zap::Note>("note")
    .resources<"note">(ProfileInfo::cResType_Course)
    .flag(Profile::cFlag_DrawCullCheck)
    .createInfo(&cCreateInfo)
    .build();

zap::Note::Note(const ActorCreateParam& param)
    : ActorMultiState(param)
    , mModel(nullptr)
    , mClefParent()
    , mManagerID()
    , mCollected(false)
{ }

ActorBase::Result zap::Note::create() {
    tk::print("Note created\n");

    mModel = AnimModel::create("note", "note", 0, 1);
    mModel->playTexAnim("rainbow");
    mModel->getTexAnim(0)->getFrameCtrl().setPlayMode(FrameCtrl::cMode_Repeat); // TODO: Check if this is necessary, it might repeat by default
    mScale = sead::Vector3f(cScaleFactor, cScaleFactor, cScaleFactor);

    mCollisionCheck.set(this, cCollisionData);
    
    // Setting: Manager ID
    mManagerID = (red::SpriteUtil::getNybble1(this) << 4) | red::SpriteUtil::getNybble2(this);
    
    // Movement setup
    const u8 nybble20 = red::SpriteUtil::getNybble20(this);
    if (nybble20 > cPos_KinokoLift) {
        tk::fatal("Movement type was out of bounds");
    }
    const ParentMovementType movementType = static_cast<ParentMovementType>(nybble20);
    u32 movementMask = mMovementHandler.getTypeMask(movementType);
    mMovementHandler.link(mPos, movementMask, mParamEx.course.movement_id); // nybble 21-22

    changeState(StateID_Idle);
    
    updateModel();

    return cResult_Success;
}

bool zap::Note::execute() {
    mMovementHandler.execute();
    mPos = mMovementHandler.getPosition();

    executeState();
    
    return true;
}

bool zap::Note::draw() {
    if (isState(StateID_Active) || isState(StateID_Collecting))
        mModel->draw();
    return true;
}

void zap::Note::updateModel() {
    mModel->update(mPos, mAngle, mScale);
}

void zap::Note::collect() { 
    if (mCollected) {
        return;
    }

    if (isState(StateID_Active)) {
        changeState(StateID_Idle); 
    }

    // TODO: add collect anim or whatever (StateID_Collecting?)

    // TODO: notify parent
    ActorBase* parent = ActorMgr::instance()->getActorPtr(mClefParent);
    if (parent != nullptr) {
        Clef* clef = static_cast<Clef*>(parent);
        clef->noteCollected();
    } else {
        tk::println("FAILED TO NOTIFY PARENT");
    }
}

void zap::Note::reset() { }

/** STATE: Idle */

void zap::Note::initializeState_Idle() {
    tk::println(":3 Idle");
}

void zap::Note::executeState_Idle() { }

void zap::Note::finalizeState_Idle() { }


/** STATE: Active */

void zap::Note::initializeState_Active() { 
    // so this is called when the note becomes active
    tk::println(":3 Active");

    reviveCollisionCheck();
    
    mCollected = false;
    //TODO: here add logic for animation when the note spawns (idk some rotate? check odyssey)
}

void zap::Note::executeState_Active() { 
    updateModel();
}

void zap::Note::finalizeState_Active() { 
    removeCollisionCheck();
}


/** STATE: Collecting */

void zap::Note::initializeState_Collecting() { 
    tk::println(":3 Collecting");
}

void zap::Note::executeState_Collecting() { 
    //TODO: logic for animating the note when its collected
    // when done switch back to idle and reset!
    updateModel();
}

void zap::Note::finalizeState_Collecting() {
    
}



// Todo: note collect effect RP_DRCStar_TouchGet scaled down 0.25
