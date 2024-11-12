#include "PvPSetting.h"

#include "SettingVariable.h"

void fq::client::PvPSetting::OnStart()
{
	SettingVariable::IsAllowOtherPlayerAttack = mbIsOn;
}

std::shared_ptr<fq::game_module::Component> fq::client::PvPSetting::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAssist = std::dynamic_pointer_cast<PvPSetting>(clone);

	if (cloneAssist == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAssist = game_module::ObjectPool::GetInstance()->Assign<PvPSetting>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAssist = *this;
	}

	return cloneAssist;
}
