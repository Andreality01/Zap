#include <zap/actor/Clef.h>
#include <audio/GameAudio.h>
#include <zap/Zap.h>
#include <effect/EffectID.h>
#include <imgui/imgui.h>
#include <effect/EffectCreateUtil.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::Clef, Actor);

static constexpr f32 cScaleFactor = 0.17f;
static constexpr f32 cCollectAnimDuration = 9.0f; // frames
static constexpr f32 cCollectAnimTiles = 2.0f;

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
    .kind = CC::cKind_Enemy,
    .attack = CC::cAttack_None,
    .vs_kind = CC::TargetKind(
        CC::cTargetKind_Player
    ),
    .vs_damage = CC::cDamageFrom_All,
    .status = CC::cStatus_None,
    .callback = [](ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) { 
        zap::Clef* self = cc_self->getOwner<zap::Clef>();
        if (self != nullptr)
            self->collect();
    }
};

const Profile* zap::Clef::cProfile = zap::getRegistrar()->newProfile<zap::Clef>("clef")
    .resources<"clef">(ProfileInfo::cResType_Course)
    .createInfo(&cCreateInfo)
    .flag(Profile::cFlag_DrawCullCheck)
    .build();

// TODO: 
// Powerup
// ID
// target count
// collected count
// time limit
// touched (bool)
// movement handler
// allow respawn
zap::Clef::Clef(const ActorCreateParam& param)
    : Actor(param)
    , mClefModel(nullptr)
    , mCollected(false)
    , mCollecting(false)
    , mCollectAnimProgress(0.0f)
    , mBaseYPos(0.0f)
    , mBaseScale(0.0f)
    , mTime(0.0f)
    , mEffect1()
    , mEffect2()
    , mEffect3()
    , mEffect4()
{ }

ActorBase::Result zap::Clef::create() {
    mClefModel = AnimModel::create("clef", "clef", 0, 1);
    mClefModel->playTexAnim("rainbow");
    mClefModel->getTexAnim(0)->getFrameCtrl().setPlayMode(FrameCtrl::cMode_Repeat); // TODO: Check if this is necessary, it might repeat by default

    mScale = sead::Vector3f(cScaleFactor, cScaleFactor, cScaleFactor);
    
    mBaseYPos = mPos.y;
    mBaseScale = mScale.x;

    mCollisionCheck.set(this, cCollisionData);
    reviveCollisionCheck();

    // TODO: movement handler
    updateModel();

    return cResult_Success;
}

bool zap::Clef::execute() {
    if (!mCollected) {
        updateModel();
        mEffect1.createEffect(RP_Mario_Star_3, &mPos, nullptr/*, &effectScale*/);
    }

    // static u32 effectID = RP_UI_Player_Star_2;
    // if (ImGui::Begin("Effect")) {
    //     ImGui::InputInt("ID", (s32*)&effectID, 1);
    // } ImGui::End();
    // mEffect1.createEffect(effectID, &mPos);
    mTime++;
        
    if (mCollecting) {
        mCollectAnimProgress += 1.0f / cCollectAnimDuration;
        
        mPos.y = mBaseYPos + sead::Mathf::sin(mCollectAnimProgress) * (cCollectAnimTiles * 16.0f); // 1 tile = 16 pos units
        
        mAngle.y() += sead::Mathf::deg2idx(10.0f); // 10 degrees per frame
        
        // if (mCollectAnimProgress >= sead::Mathf::pi() / 1.5f) { // start late
        //     mScale.x = sead::Mathf::lerp(mBaseScale, 0.0f, (mCollectAnimProgress - (sead::Mathf::pi() / 1.5f)) * 3.33f / sead::Mathf::pi()); // catch up
        //     mScale.y = mScale.z = mScale.x;
        // }

        // Effect
        sead::Vector3f effectScale = sead::Vector3f(0.25f, 0.25f, 0.25f);
        mEffect2.createEffect(RP_Coinedit_StarCoin_on, &mPos, nullptr, &effectScale);
        mEffect3.createEffect(RP_AssistBlock_MarkLine, &mPos, nullptr, &effectScale);
        mEffect4.createEffect(RP_CSelect_StarGate_Fall, &mPos, nullptr, &effectScale);

        
        if (mCollectAnimProgress >= sead::Mathf::pi()) { // pi = 1 full wave (up then down)
            mCollecting = false;
            mCollected = true;
        }
    } else {
        mPos.y = sead::Mathf::sin(mTime * (1.0f / 60.0f)) * 6.0f + mBaseYPos;
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
    if (mCollected || mCollecting) 
        return;

    mCollecting = true;

    GameAudio::getAudioObjMap()->startSound("SE_SYS_RED_RING", mPos);

    //EffectCreateUtil::createEffect(RP_DRCStar_CountFlash, &mPos);

    removeCollisionCheck();
}

// Todo: sound effect


// Todo: when collect all effect: RP_CSelect_StarCoin_Open
