#include "Socket.h"

#include "Transform.h"
#include "SkinnedMeshRenderer.h"
#include "Animator.h"

fq::game_module::Socket::Socket()
	:mTransform(nullptr)
	, mNodeHierarchyInstance(nullptr)
	, mBoneIndex(0)
	, mBoneName{}
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
	if (mNodeHierarchyInstance)
	{
		auto matrix = mNodeHierarchyInstance->GetRootTransform(mBoneIndex);

		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion rotation;

		// decompose 실패 시 위치만 맞춰주도록 처리
		if (matrix.Decompose(scale, rotation, pos))
		{
			mTransform->GenerateLocal(pos, rotation, mTransform->GetLocalScale());
		}
		else
		{
			mTransform->SetLocalPosition(matrix.Translation());
		}
	}
	else
	{
		BindBone();
	}
}

void fq::game_module::Socket::OnStart()
{
	BindBone();
}

void fq::game_module::Socket::BindBone()
{
	mTransform = GetComponent<Transform>();

	auto parent = GetGameObject()->GetParent();

	if (parent && parent->HasComponent<game_module::Animator>())
	{
		auto animator = parent->GetComponent<game_module::Animator>();

		auto boneHierarchyInstance = animator->GetSharedNodeHierarchyInstance();

		if (boneHierarchyInstance != nullptr)
		{
			auto boneHierarchy = boneHierarchyInstance->GetNodeHierarchy();

			const auto& bones = boneHierarchy->GetBones();

			bool check = boneHierarchy->TryGetBoneIndex(GetBoneName(), &mBoneIndex);

			if (check)
			{
				mNodeHierarchyInstance = boneHierarchyInstance;
			}
		}
	}
}
