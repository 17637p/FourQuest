#include "TextUI.h"

std::shared_ptr<fq::game_module::Component> fq::game_module::TextUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto textUI = std::dynamic_pointer_cast<TextUI>(clone);

	if (textUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		textUI = ObjectPool::GetInstance()->Assign<TextUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*textUI = *this;
	}

	return textUI;
}
