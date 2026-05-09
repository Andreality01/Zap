#include <actor/ActorMgr.h>
#include <actor/Profile.h>
#include <player/PlayerBase.h>
#include <telkin/Print.h>
#include <zap/Zap.h>
#include <zap/actor/Cataquack.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::Cataquack, Enemy)

CREATE_STATE_ID(zap::Cataquack, Walk)
CREATE_STATE_ID(zap::Cataquack, Turn)
CREATE_STATE_ID(zap::Cataquack, Launch)

using CC = ActorCollisionCheck;
const CC::CollisionData zap::Cataquack::cCollisionData = {
    .center_offset = { 0.0f, 14.0f },
    .half_size = { 11.0f, 14.0f },
    .shape_type = CC::cShapeType_Box,
    .kind = CC::cKind_Enemy,
    .attack = CC::cAttack_None,
    .vs_kind = CC::TargetKind(CC::cTargetKind_Player | CC::cTargetKind_Yoshi | CC::cTargetKind_ChibiYoshi),
    .vs_damage = CC::DamageFrom(CC::cDamageFrom_Star),
    .status = CC::cStatus_None,
    .callback = [](CC* cc_self, CC* cc_other) {
        const Actor* other = cc_other->getOwner();
        const ActorType otherType = other->getActorType();
        if (otherType != cActorType_Player && otherType != cActorType_Yoshi) {
            return;
        }
        const ActorUniqueID otherID = other->getActorUniqueID();
        Cataquack* self = static_cast<Cataquack*>(cc_self->getOwner());
        PlayerBase* player = static_cast<PlayerBase*>(ActorMgr::instance()->getActorPtr(otherID));

        // TODO: Detect between player riding Yoshi vs. lone Yoshi
        //if (otherType == cActorType_Yoshi && !player->isStatus(PlayerBase::cStatus_RidePlayer)) {
        //    return;
        //}
        self->mTarget = otherID;
        self->mTargetInitialY = player->getPos().y;
        self->changeState(Cataquack::StateID_Launch);
    }
};

const ActorCreateInfo zap::Cataquack::cCreateInfo = {
    .offset_x = 8, .offset_y = -8,
    .spawn_range = {
        .offset_x = 8, .offset_y = -8,
        .half_size_x = 32, .half_size_y = 32
    },
    .cull_range = {
        .up = 0, .down = 0, .left = 0, .right = 0 },
    .flag = 0,
};

constexpr f32 cScaleFactor = 0.1f;
constexpr f32 cAnimBlendTime = 10.0f;
constexpr f32 cDefaultLaunchHeight = 8.0f; // in tiles

Profile* zap::Cataquack::cProfile = zap::getRegistrar()->newProfile<zap::Cataquack>("cataquack")
    .resources<"poihana">(ProfileInfo::cResType_Course)
    .createInfo(&cCreateInfo)
    .flag(Profile::Flag::cFlag_DrawCullCheck)
    .build();

zap::Cataquack::Cataquack(const ActorCreateParam& param)
    : Enemy(param)
    , mModel(nullptr)
    , mChasing(false)
    , mTargetInitialY(0)
    , mLaunchHeight(cDefaultLaunchHeight * 16.0f) // TODO: this actually doesnt really work, it gets affected by things that change the player's gravity like minimush
{ }

//* ===== Primary Functions ===== *//

ActorBase::Result zap::Cataquack::create() {
    this->mModel = JointBlendModel::create("poihana", "poihana", 3);
    this->mScale = { cScaleFactor, cScaleFactor, cScaleFactor };
    this->mModel->setAnm("walk", cAnimBlendTime);

    // sensors
    static const ActorBgCollisionCheck::Sensor foot = { -6.0f, 6.0f, 0.0f };
    static const ActorBgCollisionCheck::Sensor head = { -6.0f, 6.0f, 28.0f };
    static const ActorBgCollisionCheck::Sensor wall = { 8.0f, 24.0f, 12.0f };
    this->mBgCheckObj.set(this, &foot, &head, &wall);

    this->mDirection = this->getPlayerDirLR();
    this->mAngle.y() = cBaseAngleY[this->mDirection];

    this->mCollisionCheck.set(this, Cataquack::cCollisionData);
    this->reviveCollisionCheck();

    // TODO: investigate if Cloud collider type actually does anything useful
    /*this->mCollider.set(this, {
        .pos_offset = { 0.0f, 14.0f },
        .rot_pivot_offset = { 0.0f, 0.0f },
        .left_top_offset = { -5.5f, 10.0f },
        .right_under_offset = { 5.5f, -12.0f },
        .angle = 0,
    });
    this->mCollider.setType(ActorBoxBgCollision::Type::cType_Cloud);
    ColliderMgr->add(this->mCollider);
    this->mCollider.execute(); // call in execute */

    this->changeState(Cataquack::StateID_Walk);
    this->calcMdl_Normal();
    return cResult_Success;
}

bool zap::Cataquack::execute() {
    this->controlLaunchHeight();

    this->executeState();
    this->calcMdl_Normal();
    this->screenOutCheck(0);
    return true;
}

bool zap::Cataquack::draw() {
    this->mModel->draw();
    return true;
}

void zap::Cataquack::calcMdl_Base() {
    this->mModel->update(
        this->mPos + sead::Vector3f(0.0f, 20.0f, 0.0f),
        this->mAngle,
        this->mScale
    );
}

//* ===== Utility Functions ===== *//

void zap::Cataquack::handleMovement() {
    this->calcSpeedY_(); // apply gravity
    this->posMove_(); // apply velocity
    this->bgCheck_(); // Terrain collision
    if (this->bgCheckFoot_()) {
        this->mSpeed.y = 0.0f; // stop accelerating downwards if we hit the ground
    }
}

void zap::Cataquack::setChaseMode(bool active) {
    this->mChasing = active;

    const f32 speedBase = active ? 1.0f : 0.5f;
    this->mSpeed.x = speedBase * cEnMuki[this->mDirection];
    this->mSpeedMax.x = speedBase * cEnMuki[this->mDirection];
    this->mModel->getCurSklAnim()->getFrameCtrl().setRate(speedBase * 2);
}

void zap::Cataquack::controlLaunchHeight() {
    if (!this->mTarget.isValid()) {
        return;
    }
    
    PlayerBase* target = static_cast<PlayerBase*>(ActorMgr::instance()->getActorPtr(this->mTarget));
    if (target == nullptr || target->getPos().y <= this->mTargetInitialY + this->mLaunchHeight) {
        return;
    }
    // TODO: make this smoother (how does bouncy cloud do it?)
    target->getSpeedVec().y *= 0.3f;
    this->mTarget.invalidate();
}

//* ===== State Functions ===== *//

/** STATE: Walk */

void zap::Cataquack::initializeState_Walk() {
    const f32 speed = this->mChasing ? 1.0f : 0.5f;
    this->mSpeed.x = speed * cEnMuki[this->mDirection];
    this->mSpeedMax.x = speed * cEnMuki[this->mDirection];
}

void zap::Cataquack::executeState_Walk() {
    this->handleMovement();

    sead::Vector2f distToPlayer;
    if (this->searchNearPlayer(distToPlayer) != -1 && sead::Mathf::abs(distToPlayer.x) < 8.0f * 16 && sead::Mathf::abs(distToPlayer.y) < 6.0f * 16) {
        if ((this->mDirection == cDirType_Left && distToPlayer.x < 0) || (this->mDirection == cDirType_Right && distToPlayer.x > 0)) {
            this->setChaseMode(true);
        }
        if (this->mChasing) {
            if ((this->mDirection == cDirType_Left && distToPlayer.x > 0) || (this->mDirection == cDirType_Right && distToPlayer.x < 0)) {
                this->changeState(Cataquack::StateID_Turn);
            }
        }
    } else {
        this->setChaseMode(false);
    }

    if (this->mBgCheckObj.checkWall(this->mDirection) && !this->mChasing) {
        this->changeState(Cataquack::StateID_Turn);
    }
}

void zap::Cataquack::finalizeState_Walk() { }

/** STATE: Turn */

void zap::Cataquack::initializeState_Turn() {
    this->mDirection = ::InvDirX(this->mDirection);
    this->mSpeed.x = 0;
    this->mSpeedMax.x = 0;
}

void zap::Cataquack::executeState_Turn() {
    this->handleMovement();

    u32 step = sead::Mathf::deg2idx(this->mChasing ? 6.0f : 3.0f);
    if (this->mAngle.y().chaseRest(cBaseAngleY[this->mDirection], step)) {
        this->changeState(Cataquack::StateID_Walk);
    }
}

void zap::Cataquack::finalizeState_Turn() {
    this->mAngle.y() = cBaseAngleY[this->mDirection];
}

/** STATE: Launch */

void zap::Cataquack::initializeState_Launch() {
    this->mDirection = this->getPlayerDirLR();
    this->mAngle.y() = cBaseAngleY[this->mDirection];

    this->mModel->setAnm("throw", cAnimBlendTime, FrameCtrl::cMode_NoRepeat);
    this->mSpeed.x = 0;
    PlayerBase* player = static_cast<PlayerBase*>(ActorMgr::instance()->getActorPtr(this->mTarget));
    player->getSpeedVec().y = 1.0f + this->mLaunchHeight / 14.0f;

    player->startSound("SE_PLY_RIDE_CLOUD");
}

void zap::Cataquack::executeState_Launch() {
    this->handleMovement();

    if (this->mModel->getCurSklAnim()->getFrameCtrl().isStop()) {
        this->changeState(Cataquack::StateID_Walk);
    }
}

void zap::Cataquack::finalizeState_Launch() {
    this->mModel->setAnm("walk", cAnimBlendTime);
}
