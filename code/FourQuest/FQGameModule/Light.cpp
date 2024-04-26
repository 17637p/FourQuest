#include "Light.h"

fq::game_module::Light::~Light()
{

}

fq::game_module::Light::Light()
	:mbIsChanged(false)
	, mInfomation{}
{}

entt::meta_handle fq::game_module::Light::GetHandle()
{
	return *this;
}

void fq::game_module::Light::SetLightType(LightType type)
{
	mInfomation.type = type;
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
