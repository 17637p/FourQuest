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
		/// 복사복을 반환합니다 
		/// </summary>
		Component* Clone(Component* clone /* = nullptr */)const override;

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
		/// 먼 평면 거리를 설정합니다
		/// </summary>
		void SetFarPlain(float distance);

	private:
		entt::meta_handle GetHandle() override;

	private:
		fq::graphics::CameraInfo mCameraInfomation;

		bool mbIsMain;
	};

}
