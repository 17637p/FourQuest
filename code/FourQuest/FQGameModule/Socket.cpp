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

	if (cloneSocket == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneSocket = ObjectPool::GetInstance()->Assign<Socket>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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

		// decompose ���� �� ��ġ�� �����ֵ��� ó��
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
