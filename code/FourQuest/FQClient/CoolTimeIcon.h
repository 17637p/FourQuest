#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/EventHandler.h"

namespace fq::game_module
{
	class ImageUI;
	class ScreenManager;
	class Camera;
}

namespace fq::client
{
	class CoolTimeIcon : public game_module::Component
	{
	public:
		CoolTimeIcon();
		~CoolTimeIcon();

		CoolTimeIcon(const CoolTimeIcon& other);
		CoolTimeIcon& operator=(const CoolTimeIcon& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void eventProcesInitCoolTime();
		void setIcon(bool isOn);

	private:
		int mPlayerID;
		int mPlayingIcon; // ������� ������ �ε���

		DirectX::SimpleMath::Vector3 mWorldOffset;
		float mScreenOffset;

		float mCurTime;
		float mFadeInTime;
		float mFadeOutTime;
		float mSpeed;

		std::vector<game_module::ImageUI*> mSkillIconImages;

		game_module::ScreenManager* mScreenManager;
		game_module::Camera* mMainCamera;

		game_module::EventHandler mInitCoolTimeHandler;

	private:
		friend void RegisterMetaData();
	};
}

/*

���� A ���
���� R ����

������ A ������
������ R ������

������ A �ֵθ���
������ R ���� ���

�ü� X ������
�ü� A ������

*/