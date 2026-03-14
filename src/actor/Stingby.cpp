#include <enemy/Enemy.h>
#include <graphics/JointBlendModel.h>
#include <graphics/SkeletalAnimation.h>
#include <graphics/Renderer.h>
#include <Zap.h>

namespace zap {
    
    class Stingby : public Enemy {
        SEAD_RTTI_OVERRIDE(Stingby, Enemy)
        
    public:
        static Profile* cProfile;
        
        Stingby(const ActorCreateParam& param);
        ~Stingby() override = default;
        
        Result create() override;
        bool execute() override;
        bool draw() override;
        
        static inline ActorCollisionCheck::CollisionData cCollisionData = {
            .center_offset = { 0.0f, 0.0f },
            .half_size = { 8.5f, 8.5f },
            .shape_type = ActorCollisionCheck::cShapeType_Box,
            .kind = ActorCollisionCheck::cKind_Enemy,
            .attack = ActorCollisionCheck::cAttack_None,
            .vs_kind = ActorCollisionCheck::cTargetKind_Player,
            .vs_damage = ActorCollisionCheck::cDamageFrom_All,
            .status = ActorCollisionCheck::cStatus_None,
            .call_back = &Enemy::normal_collcheck
        };
        
        void vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) override;
        
        DECLARE_STATE_ID(Stingby, Idle)
        DECLARE_STATE_ID(Stingby, Notice)
        DECLARE_STATE_ID(Stingby, Chase)
        
        //DECLARE_STATE_VIRTUAL_ID_OVERRIDE(Stingby, DieFall)
        DECLARE_STATE_VIRTUAL_ID_OVERRIDE(Stingby, DieOther)
        
        JointBlendModel* mModel;
        sead::Vector3f mIdleCenter;
    };
    
}

SEAD_RTTI_OVERRIDE_IMPL(zap::Stingby, Enemy)

CREATE_STATE_ID(zap::Stingby, Idle)
CREATE_STATE_ID(zap::Stingby, Notice)
CREATE_STATE_ID(zap::Stingby, Chase)
CREATE_STATE_VIRTUAL_ID_OVERRIDE(zap::Stingby, Enemy, DieOther)
//CREATE_STATE_VIRTUAL_ID_OVERRIDE(zap::Stingby, Enemy, DieFall)

Profile* zap::Stingby::cProfile = zap::getRegistrar()->newProfile<zap::Stingby>("stingby")
    .resources<"hacchin000">(ProfileInfo::cResType_Course)
    .build();

zap::Stingby::Stingby(const ActorCreateParam& param)
    : Enemy(param)
{ }

ActorBase::Result zap::Stingby::create() {    
    mModel = JointBlendModel::create("hacchin000", "hacchin000", 2, 0, 0, 0, 0);
    
    mDirection = DirType::cDirType_Right;
    mScale = sead::Vector3f(0.17f, 0.17f, 0.17f);
    
    mCollisionCheck.set(this, cCollisionData);
    reviveCollisionCheck();
    
    changeState(Stingby::StateID_Idle);
    
    return cResult_Success;
}

bool zap::Stingby::execute() {
    executeState();
    
    bool alive = true; // TODO: check state
    if (alive) {
        sead::Mathu::chase((u32*)&mAngle.y(), cBaseAngleY[mDirection], 0x11FFFFF);
    }
    
    sead::Matrix34f mtx;
    mtx.makeRTIdx(mAngle, mPos);
    
    // TODO: forwarders to get rid of this getter
    mModel->getModel()->setMtxRT(mtx);
    mModel->getModel()->setScale(mScale);
    mModel->getModel()->calcMdl();
    mModel->getModel()->calcAnm();
    
    return true;
}

bool zap::Stingby::draw() {
    // TODO: just make a member func
    Renderer::instance()->drawModel(mModel);
    
    return true;
}

void zap::Stingby::vsPlayerHitCheck_Normal(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) {
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

/** STATE: Idle */

void zap::Stingby::initializeState_Idle() {
    mIdleCenter = mPos;
    mDirection = cDirType_Right;
    mModel->setAnm("fly_idle", 10.0f);
}

void zap::Stingby::executeState_Idle() {
    sead::Vector2f player;
    searchNearPlayer(player);
    
    if (sead::Mathf::abs(player.x) < 5.5f * 16) {
        changeState(StateID_Notice);
    }
    
    if (mDirection == cDirType_Right) {
        mPos.x += 0.5f;
        if (mPos.x > mIdleCenter.x + 4 * 16) {
            mDirection = cDirType_Left;
        }
    } else {
        mPos.x -= 0.5f;
        if (mPos.x < mIdleCenter.x - 4 * 16) {
            mDirection = cDirType_Right;
        }
    }
}

void zap::Stingby::finalizeState_Idle() { }

/** STATE: Notice */

void zap::Stingby::initializeState_Notice() {
    mModel->setAnm("notice", 3.0f);
}

void zap::Stingby::executeState_Notice() {
    if (mModel->getCurSklAnim()->getFrameCtrl().isEndFrame()) {
        changeState(StateID_Chase);
    }
}

void zap::Stingby::finalizeState_Notice() { }

/** STATE: Chase */

void zap::Stingby::initializeState_Chase() {
    mModel->setAnm("fly_dash", 10.0f);
}

void zap::Stingby::executeState_Chase() {
    sead::Vector2f player;
    searchNearPlayer(player);
    
    if (sead::Mathf::abs(player.x) > 6 * 16) {
        changeState(StateID_Idle);
    }
    
    mPos.x += player.x > 0 ? 0.5f : -0.5f;
    mDirection = player.x > 0 ? cDirType_Right : cDirType_Left;
}

void zap::Stingby::finalizeState_Chase() { }

/** STATE: DieOther */

void zap::Stingby::initializeState_DieOther() {
    mModel->setAnm("die_squish", 0.0f);
    removeCollisionCheck();
}

void zap::Stingby::executeState_DieOther() {
    if (mModel->getCurSklAnim()->getFrameCtrl().isEndFrame()) {
        deleteRequest();
    }
}

void zap::Stingby::finalizeState_DieOther() { }
