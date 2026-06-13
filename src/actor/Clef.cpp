#include <zap/Zap.h>
#include <zap/actor/Clef.h>
#include <zap/actor/Note.h>
#include <audio/GameAudio.h>
#include <actor/ActorMgr.h>
#include <map/SwitchFlagMgr.h>
#include <red/util/SpriteUtil.h>
#include <effect/EffectID.h>
#include <effect/EffectCreateUtil.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::Clef, ActorMultiState);

static constexpr f32 cScaleFactor = 0.17f;
static constexpr f32 cCollectAnimDuration = 9.0f; // frames
static constexpr f32 cCollectAnimTiles = 1.5f;

static constexpr sead::SafeArray<u16, 9> cRewards = { 591, 592, 594, 593, 595, 596, 598, 597, 599 };

const ActorCreateInfo zap::Clef::cCreateInfo = {
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
const CC::CollisionData zap::Clef::cCollisionData = {
    .center_offset = { 0.0f, 0.0f },
    .half_size = { 8.0f, 16.0f },
    .shape_type = CC::ActorCollisionCheck::cShapeType_Box,
    .kind = CC::cKind_Item, 
    .attack = CC::cAttack_None,
    .vs_kind = CC::TargetKind(
        CC::cTargetKind_Player
    ),
    .vs_damage = CC::cDamageFrom_All,
    .status = CC::cStatus_None,
    .callback = [](ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) {
        tk::println("Collision to clef by %u", cc_other->getOwner()->getActorType());
            
        zap::Clef* self = cc_self->getOwner<zap::Clef>();
        if (self != nullptr) {
            tk::println("Triggering collect!");
            self->collect();
        }
    }
};

Profile* zap::Clef::sProfile = zap::getRegistrar()->newProfile<zap::Clef>("clef")
    .resources<"clef">(ProfileInfo::cResType_Course)
    .createInfo(&cCreateInfo)
    .flag(Profile::cFlag_DrawCullCheck)
    .build();

zap::Clef::Clef(const ActorCreateParam& param)
    : ActorMultiState(param)
    , mClefModel(nullptr)
    , mCollected(false)
    , mCollecting(false)
    , mCollectAnimProgress(0.0f)
    , mBaseScale(0.0f)
    , mTime(0.0f)
    , mStoredNoteCount(0)
    , mFoundNoteCount(0)
    , mCollectedNoteCount(0)
    , mScanAttempt(0)
    , mNotes(nullptr)
{ }

ActorBase::Result zap::Clef::create() {
    tk::println("Creating clef");
 
    // Model setup
    mClefModel = AnimModel::create("clef", "clef", 3, 0, 1);
    mClefModel->playTexSrtAnim("anim_color");
    mClefModel->playSklAnim("Wait");
    
    // Positioning
    mScale = sead::Vector3f(cScaleFactor, cScaleFactor, cScaleFactor);

    mBaseScale = mScale.x;
    
    // Movement setup
    const u8 nybble20 = red::SpriteUtil::getNybble20(this);
    if (nybble20 > cPos_KinokoLift) {
        tk::fatal("Movement type was out of bounds");
    }
    const ParentMovementType movementType = static_cast<ParentMovementType>(nybble20);
    u32 movementMask = mMovementHandler.getTypeMask(movementType);
    mMovementHandler.link(mPos, movementMask, mParamEx.course.movement_id); // nybble 21-22

    // Collision
    mCollisionCheck.set(this, cCollisionData);
    reviveCollisionCheck();

    // Event IDs
    mGameStartEventID = (red::SpriteUtil::getNybble8(this) << 4) | red::SpriteUtil::getNybble9(this);
    mGameWonEventID = (red::SpriteUtil::getNybble10(this) << 4) | red::SpriteUtil::getNybble11(this);

    // Members
    mTargetNoteCount = (red::SpriteUtil::getNybble3(this) << 4) | red::SpriteUtil::getNybble4(this);
    //tk::println("Targets %u", mTargetNoteCount);
    mManagerID = (red::SpriteUtil::getNybble1(this) << 4) | red::SpriteUtil::getNybble2(this);
    //tk::println("Manager ID %u", mManagerID);
    mRewardID = red::SpriteUtil::getNybble5(this);
    //tk::println("Reward ID %u", mRewardID);
    
    updateModel();
    
    return cResult_Success;
}

void zap::Clef::scanNotes() {
    mScanAttempt++;
    tk::println("Scanning... attempt %u", mScanAttempt);
    if (mScanAttempt >= 4) {
        tk::fatal("Couldnt find notes after 3 attempts...");
    }
    
    ActorMgr* actorMgr = ActorMgr::instance();
    // find notes
    for (auto it = actorMgr->getActorBegin(); it != actorMgr->getActorEnd(); it++) {
        if (Note* note = sead::DynamicCast<Note>(*it)) {
            if (note->getManagerID() == mManagerID) {
                mFoundNoteCount++;
                tk::println("Found note %u of %u", mFoundNoteCount, mTargetNoteCount);
            }
        }
    }
    
    // alloc
    
    u32 storedIndex = 0;
    if (mFoundNoteCount == mTargetNoteCount) {
        mNotes = new(mActorHeap) ActorUniqueID[mTargetNoteCount];
        for (auto it = actorMgr->getActorBegin(); it != actorMgr->getActorEnd(); it++) {
            if (Note* note = sead::DynamicCast<Note>(*it)) {
                if (note->getManagerID() == mManagerID) {
                    note->setParent(mActorUniqueID); // set the parent to this clef instance
                    mNotes[storedIndex] = note->getActorUniqueID();
                    storedIndex++;
                    mStoredNoteCount++;
                    tk::println("Stored note %u, found: %u", storedIndex, mStoredNoteCount);
                }
            }
        }
        
        // all notes found!
    }

    tk::println("Target note count: %u", mTargetNoteCount);
    tk::println("Found note count: %u", mFoundNoteCount);
    tk::println("Stored note count: %u", mStoredNoteCount);
    tk::println("Did find all notes?: %i", mStoredNoteCount == mTargetNoteCount);

    if (mStoredNoteCount == mTargetNoteCount && mNotes != nullptr) {
        tk::println("Success");
    } else {
        mFoundNoteCount = 0;
        mStoredNoteCount = 0;
    }
}

bool zap::Clef::execute() {
    mMovementHandler.execute();
    mPos = mMovementHandler.getPosition();

    if (mNotes == nullptr) {
        scanNotes();
    }
    
    mTime++;

    f32 yOffset = 0.0f;

    if (mCollecting) {
        mCollectAnimProgress += 1.0f / cCollectAnimDuration;

        // vertical collect movement
        yOffset = sead::Mathf::sin(mCollectAnimProgress) * (cCollectAnimTiles * 16.0f); // 1 tile = 16 pos units

        // angle
        mAngle.y() += sead::Mathf::deg2idx(15.0f + (mCollectAnimProgress)); // 10 degrees per frame

        // if (mCollectAnimProgress >= sead::Mathf::pi() / 1.5f) { // start late
        //     mScale.x = sead::Mathf::lerp(mBaseScale, 0.0f, (mCollectAnimProgress - (sead::Mathf::pi() / 1.5f)) * 3.33f / sead::Mathf::pi()); // catch up
        //     mScale.y = mScale.z = mScale.x;
        // }

        // Effect
        sead::Vector3f effectScale = sead::Vector3f(0.25f, 0.25f, 0.25f);
        //mEffect2.createEffect(RP_Coinedit_StarCoin_on, &mPos, nullptr, &effectScale);
        //mEffect3.createEffect(RP_AssistBlock_MarkLine, &mPos, nullptr, &effectScale);
        //mEffect4.createEffect(RP_CSelect_StarGate_Fall, &mPos, nullptr, &effectScale);


        if (mCollectAnimProgress >= sead::Mathf::pi()) { // pi = 1 full wave (up then down)
            mCollecting = false;
            mCollected = true;
        }
    }

    mPos.y += yOffset;
    
    if (!mCollected) {
        updateModel();
        //mEffect1.createEffect(RP_Mario_Star_3, &mPos, nullptr/*, &effectScale*/);
    }

    return true;
}

bool zap::Clef::draw() {
    if (!mCollected)
        mClefModel->draw();

    return true;
}

void zap::Clef::updateModel() {
    mClefModel->update(mPos, mAngle, mScale);
}

void zap::Clef::collect() {
    if (!isReady()) 
        return;

    if (mCollected || mCollecting)
        return;

    mCollecting = true;

    GameAudio::getAudioObjMap()->startSound("SE_SYS_RED_RING", mPos);

    // Set all music notes with this parent ID to state of collecting

    //EffectCreateUtil::createEffect(RP_DRCStar_CountFlash, &mPos);
    
    for (u32 i = 0; i < mTargetNoteCount; i++) {
        ActorUniqueID uniqueID = mNotes[i];
        ActorBase* actorPtr = ActorMgr::instance()->getActorPtr(uniqueID);
        if (actorPtr != nullptr) {
            Note* note = static_cast<Note*>(actorPtr); // we static cast here because we already know its 100% a Note and its alive
            note->changeState(Note::StateID_Active); 
            //tk::println("Changed the state of a note");
        } else {
            //tk::println("Failed to lookup");
        }
    }

    removeCollisionCheck();

    tk::println("Game started! triggering %u", mGameStartEventID);
    SwitchFlagMgr::instance()->set(mGameStartEventID, 0, true);
} 

void zap::Clef::noteCollected() {
    mCollectedNoteCount++;
    tk::println("Collected %u", mCollectedNoteCount);

    // TODO: store the count of notes collected and play a different pitch based on the # 
    
    if (mCollectedNoteCount >= mTargetNoteCount) {
        tk::println("Game won! triggering %u", mGameWonEventID);
        SwitchFlagMgr::instance()->set(mGameWonEventID, 0, true);

        // give powerup reward
        // 0 - none,

        if (mRewardID == 0 || mRewardID > 9)
            return;
        
        // else reward a powerup
        ActorCreateParam info;
        info.param_0 = 0x6000000;
        info.profile = Profile::get(cRewards[mRewardID - 1]);

        ActorMgr::instance()->createImmediately(info); // 0 index
    }
}

// Todo: sound effect
// Todo: when collect all effect: RP_CSelect_StarCoin_Open


/***
 * 1. Timer logic (nybble)
    * Retry (nybble)
    * 
 * 2. Multiple rounds of notes
 * 4. Movement controller
 * 5. Animations & model
 * 6. Sound effects (timer, collection) //chord scale
 * 7. Event activation
 */


/**
 * Event activation notes
 * 1. Game start (clef collected)
 * 2. Game won
 * 3. (future) phase shift
 */
