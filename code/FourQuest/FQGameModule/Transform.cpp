#include "Transform.h"

#include <queue>

FQ_REGISTRATION
{
	entt::meta<fq::game_module::Transform>()
		.type(entt::hashed_string("Transform"))
		.base<fq::game_module::Component>()
		.data<&fq::game_module::Transform::mPosition>(entt::hashed_string("mPosition"))
		.prop(fq::reflect::tag::name,"mPosition")
		.data<&fq::game_module::Transform::mRotation>(entt::hashed_string("mRotation"))
		.prop(fq::reflect::tag::name,"mRotation")
		.data<&fq::game_module::Transform::mScale>(entt::hashed_string("mScale"))
		.prop(fq::reflect::tag::name,"mScale");

}

using namespace DirectX::SimpleMath;

fq::game_module::Transform::Transform()
	:mPosition{Vector3::Zero}
	,mRotation{Quaternion::Identity}
	,mScale{Vector3::One}
	,mLocalMatrix{Matrix::Identity}
	,mWorldMatrix{Matrix::Identity}
	,mParent(nullptr)
	,mChidren{}
{}

fq::game_module::Transform::~Transform()
{}

DirectX::SimpleMath::Vector3 fq::game_module::Transform::GetWorldPosition() const
{
	if (!mParent)
	{
		return mPosition;
	}

	return Vector3::Transform(mPosition, mParent->GetWorldMatrix());
}

void fq::game_module::Transform::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	mPosition = position;

	updateMatrix();
}

void fq::game_module::Transform::AddPosition(DirectX::SimpleMath::Vector3 deltaPosition)
{
	mPosition += deltaPosition;

	updateMatrix();
}

void fq::game_module::Transform::SetScale(DirectX::SimpleMath::Vector3 scale)
{
	mScale = scale;

	updateMatrix();
}

void fq::game_module::Transform::updateMatrix()
{
	Matrix traslationM = Matrix::CreateTranslation(mPosition);
	Matrix rotationM = Matrix::CreateFromQuaternion(mRotation);
	Matrix scaleM = Matrix::CreateScale(mScale);

	mLocalMatrix = scaleM * rotationM * traslationM;

	if (mParent)
	{
	    Matrix parentWorldMatrix = mParent->GetWorldMatrix();
		mWorldMatrix = mLocalMatrix * parentWorldMatrix;
	}
	else
	{
		mWorldMatrix = mLocalMatrix;
	}

	// 자식 트랜스폼을 계산
	for (Transform* child : mChidren)
	{
		child->updateMatrix();
	}
}

void fq::game_module::Transform::SetRotation(DirectX::SimpleMath::Quaternion rotation)
{
	mRotation = rotation;

	updateMatrix();
}

void fq::game_module::Transform::SetTransformMatrix(DirectX::SimpleMath::Matrix matrix)
{
	matrix.Decompose(mScale, mRotation, mPosition);
	updateMatrix();
}

void fq::game_module::Transform::SetParent(Transform* parent)
{
	if (parent == mParent || parent == this)
	{
		return;
	}

	mParent = parent;

	if (mParent != nullptr)
	{
		mParent->AddChild(this);
	}
}

void fq::game_module::Transform::AddChild(Transform* inChild)
{
	if (inChild == this || inChild ==nullptr)
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

	removeChild->SetParent(nullptr);
}

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

