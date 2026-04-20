#include <zap/actor/Biddybud.h>
#include <zap/Zap.h>
#include <red/util/SpriteUtil.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::Biddybud, Enemy)

static constexpr f32 cScaleFactor = 0.17f; // 3DW models are large

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
    .build();

zap::Biddybud::Biddybud(const ActorCreateParam& param)
    : Enemy(param)
    , mModel(nullptr)
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
    
    updateModel();
    
    return cResult_Success;
}

bool zap::Biddybud::execute() {
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
    mModel->update(mPos, mAngle, mScale, !isState(StateID_Ice));
}
