#include <actor/Actor.h>
#include <actor/Profile.h>

namespace zap {

class NybbleBank : public Actor {
    SEAD_RTTI_OVERRIDE(NybbleBank, Actor)

public:
    static Profile* sProfile;

    NybbleBank(const ActorCreateParam& param);
    ~NybbleBank() override = default;

private:
    static const ActorCreateInfo cCreateInfo;
};

} // namespace zap
