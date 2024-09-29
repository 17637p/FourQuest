#pragma once
#include "Component.h"

namespace fq::game_module
{
	/// <summary>
	/// ���� �����, ���� Ŭ���� �����Ͽ� ������ ó������
	/// </summary>
	class StateEventEmitter : public Component
	{
	public:
		StateEventEmitter();
		~StateEventEmitter();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
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