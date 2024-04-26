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
		/// ����Ʈ Ÿ���� ��ȯ�մϴ�
		/// </summary>
		LightType GetLightType()const { return mInfomation.type; }

		/// <summary>
		/// ����Ʈ Ÿ���� �����մϴ� 
		/// </summary>
		void SetLightType(LightType type);

		/// <summary>
		/// ����� ������ ��ȯ�մϴ� 
		/// </summary>
		float GetIntensity()const { return mInfomation.intensity; }

		/// <summary>
		/// ��⸦ ��ȯ�մϴ�
		/// </summary>
		void SetIntensity(float intensity);

		/// <summary>
		/// ����Ʈ�� ������ ��ȯ�մϴ� 
		/// </summary>
		float GetRange()const { return mInfomation.range; }

		/// <summary>
		/// ����Ʈ�� ������ �����մϴ�
		/// </summary>
		void SetRange(float range);

		/// <summary>
		/// ����Ʈ�� ������ ��ȯ�մϴ� 
		/// </summary>
		LightColor GetLightColor()const { return mInfomation.color; }

		/// <summary>
		/// ����Ʈ�� ������ �����մϴ� 
		/// </summary>
		void SetLightColor(LightColor color);

		/// <summary>
		/// ���� ����� ��ȯ�մϴ�
		/// </summary>
		Vector3 GetAttenuation()const { return mInfomation.attenuation; }

		/// <summary>
		/// ���� ����� �����մϴ� 
		/// </summary>
		void SetAttenuation(Vector3 attenuation);

		/// <summary>
		/// ����Ʈ ������ ��ȯ�մϴ� 
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