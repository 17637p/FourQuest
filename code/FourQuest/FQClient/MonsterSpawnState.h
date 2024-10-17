#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
    class MonsterSpawnState : public fq::game_module::IStateBehaviour
    {
    public:
        MonsterSpawnState();
        ~MonsterSpawnState();

    private:
        std::shared_ptr<IStateBehaviour> Clone() override;
        entt::meta_handle GetHandle() override { return *this; }

        void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

    private:

        friend void RegisterMetaData();
    };
}
