#include "Transform.h"

#include <queue>

FQ_REGISTRATION
{
	using namespace entt::literals;
	using namespace fq::game_module;

	entt::meta<Transform>()
		.type("Transform"_hs)
		.prop(fq::reflect::prop::name, "Transform")
		.data<&Transform::SetLocalPosition,&Transform::GetLocalPosition>("Position"_hs)
		.prop(fq::reflect::prop::name,"Position")
		.prop(fq::reflect::prop::comment, u8"로컬 좌표")
		.data<&Transform::SetLocalRotation, &Transform::GetLocalRotation>("Rotation"_hs)
		.prop(fq::reflect::prop::name,"Rotation")
		.data<&Transform::SetLocalScale, &Transform::GetLocalScale>("Scale"_hs)
		.prop(fq::reflect::prop::name,"Scale")
		.base<Component>();
}

using namespace DirectX::SimpleMath;

fq::game_module::Transform::Transform()
	: mFQTransform{}
	, mParent(nullptr)
	, mChidren{}
{}

fq::game_module::Transform::~Transform()
{}

fq::game_module::Component* fq::game_module::Transform::Clone(Component* clone /* = nullptr */) const
{
	Transform* cloneTransform = static_cast<Transform*>(clone);

	if (cloneTransform == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneTransform = new Transform(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneTransform = *this;
	}
	return cloneTransform;
}

entt::meta_handle fq::game_module::Transform::GetHandle()
{
	return *this;
}


void fq::game_module::Transform::SetLocalPosition(DirectX::SimpleMath::Vector3 position)
{
	GenerateLocal(position, mFQTransform.localRotation, mFQTransform.localScale);
}


void fq::game_module::Transform::SetLocalMatrix(DirectX::SimpleMath::Matrix matrix)
{
	mFQTransform.localMatrix = matrix;
	decomposeLocalMatrix();
	updateWorldMatrix();
}

void fq::game_module::Transform::SetParent(Transform* parent)
{
	if (parent == mParent || parent == this || IsDescendant(parent))
	{
		return;
	}

	// 부모와 연결을 해제합니다.
	Transform* prevParnt = mParent;

	mParent = parent;

	if (parent == nullptr)
	{
		mFQTransform.parent = nullptr;
	}
	else
	{
		mFQTransform.parent = &parent->mFQTransform;
	}

	if (mParent != nullptr)
	{
		mParent->AddChild(this);
	}

	// 이전 부모와 연결을  해제합니다.
	if (prevParnt)
	{
		prevParnt->RemoveChild(this);
	}

	updateWorldMatrix();
}

void fq::game_module::Transform::AddChild(Transform* inChild)
{
	if (inChild == this || inChild == nullptr)
	{
		return;
	}

	for (Transform* child : mChidren)
	{
		if (inChild == child)
		{
			return;
		}
	}

	Transform* prevParent = inChild->GetParentTransform();

	if (prevParent != nullptr)
	{
		prevParent->RemoveChild(inChild);
	}

	mChidren.push_back(inChild);
	inChild->SetParent(this);
}

void fq::game_module::Transform::RemoveChild(Transform* removeChild)
{
	mChidren.erase(std::remove(mChidren.begin(), mChidren.end(), removeChild)
		, mChidren.end());

	if (removeChild->GetParentTransform() == this)
	{
		removeChild->SetParent(nullptr);
	}
}



bool fq::game_module::Transform::IsDescendant(Transform* transfrom) const
{
	std::queue<const Transform*> q;

	q.push(this);

	while (!q.empty())
	{
		const Transform* tmp = q.front();
		q.pop();

		if (tmp == transfrom)
		{
			return true;
		}
		if (!tmp->IsLeaf())
		{
			for (Transform* child : tmp->GetChildren())
			{
				q.push(child);
			}
		}
	}

	return false;
}

bool fq::game_module::Transform::IsLeaf() const
{
	return mChidren.empty();
}

void fq::game_module::Transform::SetLocalRotation(Quaternion rotation)
{
	GenerateLocal(mFQTransform.localPosition, rotation, mFQTransform.localScale);
}

void fq::game_module::Transform::SetLocalScale(Vector3 scale)
{
	GenerateLocal(mFQTransform.localPosition, mFQTransform.localRotation, scale);
}

void fq::game_module::Transform::updateWorldMatrix()
{
	mFQTransform.worldMatrix = HasParent() ?
		mFQTransform.localMatrix * mParent->mFQTransform.worldMatrix
		: mFQTransform.localMatrix;

	decomposeWorldMatrix();

	// children
	for (auto child : mChidren)
	{
		child->updateWorldMatrix();
	}
}

void fq::game_module::Transform::updateLocalMatrix()
{
	mFQTransform.localMatrix = HasParent() ?
		mFQTransform.worldMatrix * mParent->mFQTransform.worldMatrix.Invert()
		: mFQTransform.worldMatrix;

	decomposeLocalMatrix();

	// children
	for (auto child : mChidren)
	{
		child->updateWorldMatrix();
	}
}

void fq::game_module::Transform::AddPosition(Vector3 deltaPosition)
{
	GenerateLocal(mFQTransform.localPosition + deltaPosition
		, mFQTransform.localRotation, mFQTransform.localScale);
}

void fq::game_module::Transform::decomposeLocalMatrix()
{
	mFQTransform.localMatrix.Decompose(mFQTransform.localPosition
		, mFQTransform.localRotation
		, mFQTransform.localScale);
}

void fq::game_module::Transform::decomposeWorldMatrix()
{
	mFQTransform.localMatrix.Decompose(mFQTransform.worldPosition
		, mFQTransform.worldRotation
		, mFQTransform.worldScale);
}

void fq::game_module::Transform::GenerateLocal(Vector3 position, Quaternion rotation, Vector3 scale)
{
	mFQTransform.localMatrix = Matrix::CreateScale(scale)
		* Matrix::CreateFromQuaternion(rotation)
		* Matrix::CreateTranslation(position);

	mFQTransform.localPosition = position;
	mFQTransform.localRotation = rotation;
	mFQTransform.localScale = scale;

	updateWorldMatrix();
}

