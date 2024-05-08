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
	fireSetLightType();
}

void fq::game_module::Light::SetRange(float range)
{
	mInfomation.range = range;
}

void fq::game_module::Light::SetIntensity(float intensity)
{
	mInfomation.intensity = intensity;
}

void fq::game_module::Light::SetLightColor(LightColor color)
{
	mInfomation.color = color;
}

void fq::game_module::Light::SetAttenuation(Vector3 attenuation)
{
	mInfomation.attenuation = attenuation;
}

void fq::game_module::Light::SetLightInfomation(LightInfo info)
{
	mInfomation = info;
}

void fq::game_module::Light::SetSpot(float spot)
{
	mInfomation.spot = spot;
}

void fq::game_module::Light::fireSetLightType()
{
	if (GetGameObject() && GetScene())
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::SetLightType>({this});
	}
}

std::shared_ptr<fq::game_module::Component> fq::game_module::Light::Clone(std::shared_ptr<Component> clone /*= nullptr*/) const
{
	auto cloneLight = std::dynamic_pointer_cast<Light>(clone);

	if (cloneLight == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneLight = ObjectPool::GetInstance()->AssignComponent<Light>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		*cloneLight = *this;
	}

	return cloneLight;
}

