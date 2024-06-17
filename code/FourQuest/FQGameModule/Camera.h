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
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 메인 카메라인지 여부를 반환합니다
		/// </summary>
		/// <returns>메인 카메라 여부</returns>
		bool IsMain() const { return mbIsMain; }

		/// <summary>
		/// 메인 카메라로 설정합니다
		/// </summary>
		void SetMainCamera(bool isMain);

		/// <summary>
		/// FOV를 반환합니다
		/// </summary>
		/// <returns>degree 각도</returns>
		float GetFieldOfView()const;

		/// <summary>
		/// FOV를 설정합니다
		/// 0 ~ 180 Degree
		/// </summary>
		void SetFieldOfView(float degree);

		/// <summary>
		/// 근평면 거리를 반환합니다
		/// </summary>
		float GetNearPlain() const;

		/// <summary>
		/// 근평면 거리를 설정합니다
		/// </summary>
		void SetNearPlain(float distance);

		/// <summary>
		/// 먼 평면 거리를 반환합니다 
		float GetFarPlain() const;

		/// <summary>
		/// </summary>
		/// 먼 평면 거리를 설정합니다
		void SetFarPlain(float distance);

		/// <summary>
		/// 카메라 정보를 담은 구조체로 반환합니다 
		/// </summary>
		/// <returns>카메라 정보</returns>
		fq::graphics::CameraInfo GetCameraInfomation()const { return mCameraInfomation; }

		/// <summary>
		/// View 행렬을 반환합니다
		/// </summary>
		DirectX::SimpleMath::Matrix GetView();
		
		/// <summary>
		/// Projection 행렬을 반환합니다 
		/// </summary>
		DirectX::SimpleMath::Matrix GetProjection(float aspectRatio)const;

		/// <summary>
		/// viewprojection 행렬을 반환합니다
		/// </summary>
		DirectX::SimpleMath::Matrix GetViewProjection();

		/// <summary>
		/// 화면 비를 반환합니다
		/// </summary>
		float GetAspectRatio() const { return mAspectRatio; }
	
		/// <summary>
		/// 화면비를 설정합니다.
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
