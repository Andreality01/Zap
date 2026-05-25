#include <zap/actor/Note.h>
#include <zap/Zap.h>
#include <telkin/Print.h>

SEAD_RTTI_OVERRIDE_IMPL(zap::Note, Actor);

static constexpr f32 cScaleFactor = 0.17f;

Profile* zap::Note::sProfile = zap::getRegistrar()->newProfile<zap::Note>("note")
    .resources<"note">(ProfileInfo::cResType_Course)
    .flag(Profile::cFlag_DrawCullCheck)
    .build();

zap::Note::Note(const ActorCreateParam& param)
    : Actor(param)
    , mModel(nullptr)
{ }

ActorBase::Result zap::Note::create() {
    tk::print("Note created\n");

    mModel = AnimModel::create("note", "note", 0, 1);
    mModel->playTexAnim("rainbow");
    mModel->getTexAnim(0)->getFrameCtrl().setPlayMode(FrameCtrl::cMode_Repeat); // TODO: Check if this is necessary, it might repeat by default
    mScale = sead::Vector3f(cScaleFactor, cScaleFactor, cScaleFactor);

    // TODO: hitbox collider

    // TODO: movement handler

    updateModel();

    return cResult_Success;
}

bool zap::Note::execute() {
    updateModel();
    return true;

}

bool zap::Note::draw() {
    mModel->draw();
    return true;
}

void zap::Note::updateModel() {
    mModel->update(mPos, mAngle, mScale);
}

void zap::Note::collect() { }



// Todo: note collect effect RP_DRCStar_TouchGet scaled down 0.25
