#pragma once
#pragma once

#include "Component.h"

#include "../FQCommon/FQCommonGraphics.h"

namespace fq::game_module
{
	class Camera : public Component
	{
	public:
		Camera();
		~Camera();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// ���� ī�޶����� ���θ� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ī�޶� ����</returns>
		bool IsMain() const { return mbIsMain; }

		/// <summary>
		/// ���� ī�޶�� �����մϴ�
		/// </summary>
		void SetMainCamera(bool isMain);

		/// <summary>
		/// FOV�� ��ȯ�մϴ�
		/// </summary>
		/// <returns>degree ����</returns>
		float GetFieldOfView()const;

		/// <summary>
		/// FOV�� �����մϴ�
		/// 0 ~ 180 Degree
		/// </summary>
		void SetFieldOfView(float degree);

		/// <summary>
		/// ����� �Ÿ��� ��ȯ�մϴ�
		/// </summary>
		float GetNearPlain() const;

		/// <summary>
		/// ����� �Ÿ��� �����մϴ�
		/// </summary>
		void SetNearPlain(float distance);

		/// <summary>
		/// �� ��� �Ÿ��� ��ȯ�մϴ� 
		float GetFarPlain() const;

		/// <summary>
		/// </summary>
		/// �� ��� �Ÿ��� �����մϴ�
		void SetFarPlain(float distance);

		/// <summary>
		/// ī�޶� ������ ���� ����ü�� ��ȯ�մϴ� 
		/// </summary>
		/// <returns>ī�޶� ����</returns>
		fq::graphics::CameraInfo GetCameraInfomation()const { return mCameraInfomation; }

		/// <summary>
		/// View ����� ��ȯ�մϴ�
		/// </summary>
		DirectX::SimpleMath::Matrix GetView();
		
		/// <summary>
		/// Projection ����� ��ȯ�մϴ� 
		/// </summary>
		DirectX::SimpleMath::Matrix GetProjection(float aspectRatio)const;

		/// <summary>
		/// viewprojection ����� ��ȯ�մϴ�
		/// </summary>
		DirectX::SimpleMath::Matrix GetViewProjection();

		/// <summary>
		/// ȭ�� �� ��ȯ�մϴ�
		/// </summary>
		float GetAspectRatio() const { return mAspectRatio; }
	
		/// <summary>
		/// ȭ��� �����մϴ�.
		/// </summary>
		void SetAspectRatio(float aspectRatio);

	private:
		entt::meta_handle GetHandle() override;

	private:
		fq::graphics::CameraInfo mCameraInfomation;
		float mAspectRatio;
		bool mbIsMain;
	};

}
