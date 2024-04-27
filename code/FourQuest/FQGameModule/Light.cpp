#include "Light.h"

#include "Scene.h"
#include "EventManager.h"
#include "Event.h"

fq::game_module::Light::~Light()
{

}

fq::game_module::Light::Light()
	: mInfomation{}
{}

entt::meta_handle fq::game_module::Light::GetHandle()
{
	return *this;
}

void fq::game_module::Light::SetLightType(LightType type)
{
	mInfomation.type = type;

	fireSetLight(true);
}

void fq::game_module::Light::SetRange(float range)
{
	mInfomation.range = range;

	fireSetLight();
}

void fq::game_module::Light::SetIntensity(float intensity)
{
	mInfomation.intensity = intensity;

	fireSetLight();
}

void fq::game_module::Light::SetLightColor(LightColor color)
{
	mInfomation.color = color;

	fireSetLight();
}

void fq::game_module::Light::SetAttenuation(Vector3 attenuation)
{
	mInfomation.attenuation = attenuation;

	fireSetLight();
}

void fq::game_module::Light::SetLightInfomation(LightInfo info)
{
	mInfomation = info;
}

void fq::game_module::Light::SetSpot(float spot)
{
	mInfomation.spot = spot;

	fireSetLight();
}

void fq::game_module::Light::fireSetLight(bool isChangedType /*= false*/)
{
	if (GetGameObject() && GetScene())
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::SetLightInfo>({this ,isChangedType });
	}
}

fq::game_module::Component* fq::game_module::Light::Clone(Component* clone /* = nullptr */) const
{
	Light* cloneLight = static_cast<Light*>(clone);

	if (cloneLight == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneLight = new Light(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneLight = *this;
	}
	return cloneLight;
}

