#define NOMINMAX
#include "Trail.h"

#include <algorithm>

fq::game_module::Trail::Trail()
	: mTrailObjectInterface(nullptr)
{
}

std::shared_ptr<fq::game_module::Component> fq::game_module::Trail::Clone(std::shared_ptr<Component> clone) const
{
	auto cloneTrail = std::dynamic_pointer_cast<Trail>(clone);

	if (cloneTrail == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneTrail = ObjectPool::GetInstance()->Assign<Trail>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneTrail = *this;
	}

	cloneTrail->mTrailObjectInterface = nullptr;

	return cloneTrail;
}

void fq::game_module::Trail::SetParticleMaterialInfo(graphics::ParticleMaterialInfo data)
{
	mParticleMaterialInfo = data;

	if (mParticleMaterial != nullptr)
	{
		mParticleMaterial->SetInfo(mParticleMaterialInfo);
	}
}
