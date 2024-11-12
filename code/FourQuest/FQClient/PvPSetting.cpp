#include "PvPSetting.h"

#include "SettingVariable.h"

void fq::client::PvPSetting::OnStart()
{
	SettingVariable::IsAllowOtherPlayerAttack = mbIsOn;
}

std::shared_ptr<fq::game_module::Component> fq::client::PvPSetting::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAssist = std::dynamic_pointer_cast<PvPSetting>(clone);

	if (cloneAssist == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAssist = game_module::ObjectPool::GetInstance()->Assign<PvPSetting>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAssist = *this;
	}

	return cloneAssist;
}
