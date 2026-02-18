#include "actor/ActorBase.h"
#include "actor/ActorCreateParam.h"
#include "red/registry/Registrar.h"
#include <red/util/Log.h>
#include <graphics/AnimModel.h>
#include <graphics/Renderer.h>

#include <Zap.h>

namespace zap {
    
    class DemoActor : public Actor {
    public:
        static Profile* cProfile;
        
        DemoActor(const ActorCreateParam& param)
            : Actor(param)
        { }
    
    private:
        Result create() override {
            mModel = AnimModel::create("switch_koopa", "boss_koopa_ax", 0, 0, 0, 0, 0, Model::cBoundingMode_Disable, nullptr);
            updateModel();
            return cResult_Success;
        }
        
        bool execute() override {
            mAngle.y() += 0x8000000;
            updateModel();
            return true;
        }

        bool draw() override {
            Renderer::instance()->drawModel(mModel);
            return true;
        }
        
        void updateModel() {
            sead::Matrix34f mtx;
            mtx.makeRTIdx(mAngle, mPos);
            mModel->getModel()->setMtxRT(mtx);
            mModel->getModel()->setScale(sead::Vector3f(1,1,1));
            mModel->calcMdl();
        }
        
        AnimModel* mModel;
    };

}

Profile* zap::DemoActor::cProfile = zap::getRegistrar()->newProfile<zap::DemoActor>("demo_actor")
    .drawPriority(69)
    .resources<"bolt", "switch_koopa">(ProfileInfo::cResType_Course)
    .build();
