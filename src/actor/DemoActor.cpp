#include "actor/ActorBase.h"
#include "actor/ActorCreateParam.h"
#include "dynamic_libs/os_functions.h"
#include "red/registry/Registrar.h"
#include "red/registry/builder/ProfileCreateBuilder.h"

#include <graphics/AnimModel.h>
#include <graphics/Renderer.h>

#include <Zap.h>

namespace zap {
    
    class DemoActor : public Actor {
    public:
        static Profile* cProfile;
        
        DemoActor(const ActorCreateParam& param)
            : Actor(param)
        {
            OSReport("DEMOACTOR CTOR!!!\n");
        }
        
    private:
        Result create_() override {
            OSReport("DemoActor created!!\n");
            
            mModel = AnimModel::create("star_coin", "star_coinA", 0, 0, 0, 0, 0, Model::cBoundingMode_Disable, nullptr);
            
            return cResult_Success;
        }
        
        bool execute_() override {
            OSReport("EXECUTING\n");
            
            mAngle.y() += 0x8000000;
            
            sead::Matrix34f mtx;
            mtx.makeRTIdx(mAngle, mPos);
            mModel->getModel()->setMtxRT(mtx);
            mModel->getModel()->setScale(sead::Vector3f(1,1,1));
            mModel->calcMdl();
            
            return true;
        }

        bool draw_() override {
            OSReport("DRAWING\n");
            
            Renderer::instance()->drawModel(mModel);
            
            return true;
        }
        
        AnimModel* mModel;
    };

}

Profile* zap::DemoActor::cProfile = zap::getRegistrar()->profile<zap::DemoActor>("demo_actor")
    //.drawPriority(69)
    //.resources<"star_coin">(ProfileInfo::cResType_Course)
    .build();
