#pragma once
#include "Component.h"

namespace fq::game_module
{
	/// <summary>
	/// 사운드 방출기, 사운드 클립을 참조하여 방출을 처리해줌
	/// </summary>
	class StateEventEmitter : public Component
	{
	public:
		StateEventEmitter();
		~StateEventEmitter();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		virtual void OnUpdate(float dt) override;
		virtual void OnStart() override;

	private:
		entt::meta_handle GetHandle() override;

		void emit();

	private:
		bool mbIsPlayLoop;
		bool mbIsPlayStateEnter;
		float mTurm;
		bool mbUseRandomIndex;
		std::vector<std::string> mNames;

		float mElapsedTime;
		unsigned int mNameIndex;
		bool mbIsPlayOnce;

		friend void RegisterMetaData();
	};
}