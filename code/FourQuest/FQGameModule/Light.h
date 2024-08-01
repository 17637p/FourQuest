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

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

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

		/// <summary>
		/// 라이트 정보를 설정합니다 
		/// </summary>
		void SetLightInfomation(LightInfo info);

		/// <summary>
		/// Spot 의 변수를 설정합니다 
		/// </summary>
		void SetSpot(float spot);

		/// <summary>
		/// Spot을 반환합니다 
		/// </summary>
		float GetSpot()const { return mInfomation.spot; }

		/// <summary>
		/// 라이트 그림자 사용여부를 반환합니다 
		/// </summary>
		bool OnShadow() const;
		
		/// <summary>
		/// 라이트 그림자를 설정합니다 
		/// 디렉션라이트 3개까지 가능 [5.10일 기준]
		/// </summary>
		void SetShadow(bool bOnShadow);

		/// <summary>
		/// 라이트 모드을 변경합니다 
		/// </summary>
		void SetLightMode(fq::graphics::ELightMode mode);


	private:
		void fireSetLightType();
		entt::meta_handle GetHandle() override;

	private:
		LightInfo mInfomation;
		bool mbOnShadow;
	};


}