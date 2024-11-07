#include "ImagePrint.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Transform.h"

std::shared_ptr<fq::game_module::Component> fq::client::ImagePrint::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneImagePrint = std::dynamic_pointer_cast<ImagePrint>(clone);

	if (cloneImagePrint == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneImagePrint = game_module::ObjectPool::GetInstance()->Assign<ImagePrint>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneImagePrint = *this;
	}

	return cloneImagePrint;
}

fq::client::ImagePrint::ImagePrint()
	:mScreenManager(nullptr)
	, mOffsetX(0)
	, mOffsetY(0)
{
}

fq::client::ImagePrint::~ImagePrint()
{
}

void fq::client::ImagePrint::OnStart()
{
	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::ImagePrint::OnUpdate(float dt)
{
	setScaleScreen();

	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	auto fixedWidth = mScreenManager->GetFixScreenWidth();
	auto fixedHeight = mScreenManager->GetFixScreenHeight();

	float textX = (fixedWidth * 0.5f) + (mOffsetX * (fixedWidth / (float)1920) * (fixedWidth / (float)1920));
	myTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(textX, fixedHeight - mOffsetY, 0.f));
}

void fq::client::ImagePrint::setScaleScreen()
{
	// ȭ�� ũ�⿡ ���� Scale �ڵ� ���� 
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}
