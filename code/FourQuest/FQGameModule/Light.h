#pragma once
#include "Component.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::game_module
{
	class Light : public Component
	{
		using LightType = fq::graphics::ELightType;
		using LightInfo = fq::graphics::LightInfo;
		using LightColor = DirectX::SimpleMath::Color;
		using Vector3 = DirectX::SimpleMath::Vector3;

	public:
		Light();
		~Light();
		
		void OnUpdate(float dt)override;

		/// <summary>
		/// 라이트 타입을 반환합니다
		/// </summary>
		LightType GetLightType()const { return mInfomation.type; }

		/// <summary>
		/// 라이트 타입을 변경합니다 
		/// </summary>
		void SetLightType(LightType type);

		/// <summary>
		/// 밝기의 강도를 반환합니다 
		/// </summary>
		float GetIntensity()const { return mInfomation.intensity; }

		/// <summary>
		/// 밝기를 변환합니다
		/// </summary>
		void SetIntensity(float intensity);

		/// <summary>
		/// 라이트의 범위를 반환합니다 
		/// </summary>
		float GetRange()const { return mInfomation.range; }

		/// <summary>
		/// 라이트의 범위를 변경합니다
		/// </summary>
		void SetRange(float range);

		/// <summary>
		/// 라이트의 색깔을 반환합니다 
		/// </summary>
		LightColor GetLightColor()const { return mInfomation.color; }

		/// <summary>
		/// 라이트의 색상을 변경합니다 
		/// </summary>
		void SetLightColor(LightColor color);

		/// <summary>
		/// 감쇠 상수를 반환합니다
		/// </summary>
		Vector3 GetAttenuation()const { return mInfomation.attenuation; }

		/// <summary>
		/// 감쇠 상수를 변경합니다 
		/// </summary>
		void SetAttenuation(Vector3 attenuation);

		/// <summary>
		/// 라이트 정보를 반환합니다 
		/// </summary>
		LightInfo GetLightInfomation()const { return mInfomation; }

		void SetLightInfomation(LightInfo info);

	private:
		entt::meta_handle GetHandle() override;

	private:
		bool mbIsChanged;
		LightInfo mInfomation;
	};


}