#include "Socket.h"

#include "Transform.h"
#include "SkinnedMeshRenderer.h"
#include "Animator.h"

fq::game_module::Socket::Socket()
	:mTransform(nullptr)
	,mSkinnedMeshRenderer(nullptr)
	,mBoneIndex(0)
	,mBoneName{}
{}

fq::game_module::Socket::~Socket()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::Socket::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSocket = std::dynamic_pointer_cast<Socket>(clone);

	if (cloneSocket == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneSocket = ObjectPool::GetInstance()->Assign<Socket>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSocket = *this;
	}

	return cloneSocket;

}

void fq::game_module::Socket::OnUpdate(float dt)
{
	if (mSkinnedMeshRenderer)
	{
		auto matrix = mSkinnedMeshRenderer->GetSkinnedMeshObject()->GetRootTransform(mBoneIndex);
		
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion rotation;

		matrix.Decompose(scale, rotation, pos);
		
		mTransform->GenerateLocal(pos, rotation, mTransform->GetLocalScale());
	}
}

void fq::game_module::Socket::OnStart()
{
	mTransform = GetComponent<Transform>();

	auto parent = GetGameObject()->GetParent();

	if (parent && parent->HasComponent<game_module::Animator>())
	{
		auto animator = parent->GetComponent<game_module::Animator>();

		for (auto mesh : animator->GetSkinnedMeshs())
		{
			auto skinnedMeshObject = mesh->GetSkinnedMeshObject();
			bool check = skinnedMeshObject->TryGetBoneIndex(GetBoneName(), &mBoneIndex);

			if (check)
			{
				mSkinnedMeshRenderer = mesh;
				return;
			}
		}
	}
}
