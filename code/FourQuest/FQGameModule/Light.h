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
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

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

		/// <summary>
		/// ����Ʈ ������ �����մϴ� 
		/// </summary>
		void SetLightInfomation(LightInfo info);

		/// <summary>
		/// Spot �� ������ �����մϴ� 
		/// </summary>
		void SetSpot(float spot);

		/// <summary>
		/// Spot�� ��ȯ�մϴ� 
		/// </summary>
		float GetSpot()const { return mInfomation.spot; }

		/// <summary>
		/// ����Ʈ �׸��� ��뿩�θ� ��ȯ�մϴ� 
		/// </summary>
		bool OnShadow() const;
		
		/// <summary>
		/// ����Ʈ �׸��ڸ� �����մϴ� 
		/// �𷺼Ƕ���Ʈ 3������ ���� [5.10�� ����]
		/// </summary>
		void SetShadow(bool bOnShadow);

		/// <summary>
		/// ����Ʈ ����� �����մϴ� 
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