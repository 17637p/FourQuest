#include "Transform.h"

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
		if (inChild = child)
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



