#include <zap/actor/FlyBones.h>
#include <zap/Zap.h>
#include <actor/ActorMgr.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::FlyBones, Enemy);

// Spawning data
const ActorCreateInfo zap::FlyBones::cCreateInfo = {
    .offset_x = 8, .offset_y = -16,
    .spawn_range = {
        .offset_x = 0, .offset_y = 16,
        .half_size_x = 8, .half_size_y = 16
    },
    .cull_range = { 
        .up = 0, .down = 0, .left = 0, .right = 0
    },
    .flag = 0
};

// Register it
const Profile* zap::FlyBones::cProfile = zap::getRegistrar()->newProfile<zap::FlyBones>("flybones")
    .resources<"karon", "wing", "nokonokoB">(ProfileInfo::cResType_Course)
    .createInfo(&zap::FlyBones::cCreateInfo)
    .build();

// Hitbox data
using CC = ActorCollisionCheck;
const ActorCollisionCheck::CollisionData zap::FlyBones::cCollisionData = {
    .center_offset = { 0.0f, 8.0f },
    .half_size = { 8.0f, 12.0f },
    .shape_type = CC::cShapeType_Box,
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

// Main code
zap::FlyBones::FlyBones(const ActorCreateParam& param)
    : Enemy(param)
    , mBodyModel(nullptr)
    , mWingsModel(nullptr)
{ }

ActorBase::Result zap::FlyBones::create() {
    mAngle.y() = cBaseAngleY[cDirType_Left];
    
    mBodyModel = AnimModel::create("nokonokoB", "nokonokoB", 1);
    mBodyModel->playSklAnim("flyA");
    
    mWingsModel = AnimModel::create("wing", "wing", 1);
    mWingsModel->playSklAnim("wing_kuri");
    
    updateModel();
    
    mCollisionCheck.set(this, cCollisionData);
    reviveCollisionCheck();
    
    return cResult_Success;
}

bool zap::FlyBones::execute() {
    // Delete when offscreen
    screenOutCheck(0);
    
    executeState();
    
    // Turn to face the player
    sead::Vector2f player;
    if (searchNearPlayer(player) != -1) {
        // Found a player, face him
        mDirection = player.x > 0 ? cDirType_Right : cDirType_Left;
    }
    mAngle.y().chaseRest(cBaseAngleY[mDirection], sead::Mathf::deg2idx(3.0f));
    
    updateModel();
    
    return true;
}

bool zap::FlyBones::draw() {
    mBodyModel->draw();
    mWingsModel->draw();
    return true;
}

void zap::FlyBones::updateModel() {
    mBodyModel->update(mPos, mAngle, mScale);
    mWingsModel->update(mPos + sead::Vector3f(0.0f, 8.0f, 0.0f), mAngle, mScale);
}

void zap::FlyBones::loseWings() {
    // delete self
    removeCollisionCheck();
    deleteActor(true);
    
    // spawn regular dry bones
    ActorCreateParam child;
    child.profile = Profile::get(0x288); // TODO: Enum
    child.position = mPos + sead::Vector3f(0.0f, -8.0f, 0.0f);
    ActorMgr::instance()->createImmediately(child);
}

void zap::FlyBones::vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) {
    Actor* other = cc_other->getOwner();
    
    switch (fumiCheck(cc_self, cc_other, cFumiSeType_Normal)) {
        case cFumiType_Fumi:
        case cFumiType_SpinFumi: {
            loseWings();
            return;
        }
        
        case cFumiType_MameFumi: {
            return;
        }
        
        default: {
            return Enemy::vsPlayerHitCheck_Normal(cc_self, cc_other);
        }
    }
}

void zap::FlyBones::vsYoshiHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) {
    Actor* other = cc_other->getOwner();
    
    switch (fumiCheck(cc_self, cc_other, cFumiSeType_Normal)) {
        case cFumiType_Fumi: {
            loseWings();
            return;
        }
        
        default: {
            return Enemy::vsYoshiHitCheck_Normal(cc_self, cc_other);
        }
    }
}
