#include "LoadingUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"

std::shared_ptr<fq::game_module::Component> fq::client::LoadingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto loadingUI = std::dynamic_pointer_cast<LoadingUI>(clone);

	if (loadingUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		loadingUI = game_module::ObjectPool::GetInstance()->Assign<LoadingUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*loadingUI = *this;
	}

	return loadingUI;
}

void fq::client::LoadingUI::OnStart()
{
	mScreenManager = GetScene()->GetScreenManager();

	// �ʱⰪ ����
	auto Bars = GetGameObject()->GetChildren()[0]->GetChildren();
	mProgressBar = Bars[0]->GetComponent<game_module::ImageUI>();
	mProgressBarMaxWidth = 1720;

	mProgressSoul = Bars[1]->GetComponent<game_module::Transform>();

	auto guide = GetGameObject()->GetChildren()[1]->GetChildren();
	mImages = guide[0]->GetChildren();

	auto texts = guide[1]->GetChildren();
	mTitle = texts[0]->GetComponent<game_module::TextUI>();
	mContent = texts[1]->GetComponent<game_module::TextUI>();

	texts = guide[2]->GetChildren();
	mBackgroundTitle = texts[0]->GetComponent<game_module::TextUI>();
	mBackgroundContent = texts[1]->GetComponent<game_module::TextUI>();

	// ������ �� ��� Render Off
	auto textInfo = mTitle->GetTextInfo();
	textInfo.IsRender = false;
	mTitle->SetTextInfo(textInfo);

	textInfo = mContent->GetTextInfo();
	textInfo.IsRender = false;
	mContent->SetTextInfo(textInfo);

	textInfo = mBackgroundTitle->GetTextInfo();
	textInfo.IsRender = false;
	mBackgroundTitle->SetTextInfo(textInfo);

	textInfo = mBackgroundContent->GetTextInfo();
	textInfo.IsRender = false;
	mBackgroundContent->SetTextInfo(textInfo);
}

void fq::client::LoadingUI::OnUpdate(float dt)
{
	SetScaleScreen();

	// ���
	if (mCurGuideID > 5)
	{
		auto textInfo = mBackgroundTitle->GetTextInfo();
		textInfo.Text = mTexts[mCurGuideID].Title;
		textInfo.IsRender = true;
		mBackgroundTitle->SetTextInfo(textInfo);

		textInfo = mBackgroundContent->GetTextInfo();
		textInfo.Text = mTexts[mCurGuideID].Content;
		textInfo.IsRender = true;
		mBackgroundContent->SetTextInfo(textInfo);
	}
	else
	{
		auto textInfo = mTitle->GetTextInfo();
		textInfo.Text = mTexts[mCurGuideID].Title;
		textInfo.IsRender = true;
		mTitle->SetTextInfo(textInfo);

		textInfo = mContent->GetTextInfo();
		textInfo.Text = mTexts[mCurGuideID].Content;
		textInfo.IsRender = true;
		mContent->SetTextInfo(textInfo);
	}

	for (int i = 0; i < mImages.size(); i++)
	{
		mImages[i]->GetComponent<game_module::ImageUI>()->SetIsRender(0, false);
	}

	mImages[mCurGuideID]->GetComponent<game_module::ImageUI>()->SetIsRender(0, true);
}

fq::client::LoadingUI::LoadingUI()
	:mScreenManager(nullptr),
	mProgressBar(nullptr),
	mProgressSoul(nullptr),
	mProgressBarMaxWidth(0),
	mTexts(),
	mImages(),
	mContent(nullptr),
	mCurGuideID(7)
{
}

fq::client::LoadingUI::~LoadingUI()
{
}

void fq::client::LoadingUI::SetProgressBar(float progress)
{
	// Bar Percent Setting
	auto uiInfo = mProgressBar->GetUIInfomation(0);

	uiInfo.Width = mProgressBarMaxWidth * progress;
	uiInfo.XRatio = progress;

	spdlog::trace("UIInfo.Width: {}", uiInfo.Width);

	mProgressBar->SetUIInfomation(0, uiInfo);

	// Soul Position Setting
	mProgressSoul->SetLocalPosition({ uiInfo.Width - 50 , -90, 0});
}

void fq::client::LoadingUI::SetScaleScreen()
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// Scale �ڵ� ���� 
	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}



#include <fstream>
#include <string>
#include <d2d1.h>
#include <wincodec.h>

// ��Ÿ������ ����ü
struct MyCustomMetadata {
	uint32_t width;
	uint32_t height;
	uint32_t format;
	uint32_t stringLength; // ���ڿ� ����
};

// ��Ʈ�� �����͸� �����ϴ� �Լ�
HRESULT SaveBitmapWithMetadataAndString(
	ID2D1Bitmap* pBitmap,
	const WCHAR* filePath,
	MyCustomMetadata metadata,
	const std::string& customString // �߰��� ���ڿ�
)
{
	// 1. WIC ���丮 ����
	IWICImagingFactory* pWICFactory = nullptr;
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pWICFactory)
	);

	if (FAILED(hr)) {
		return hr;
	}

	IWICBitmapEncoder* pEncoder = nullptr;
	IWICStream* pStream = nullptr;
	IWICBitmapFrameEncode* pFrame = nullptr;

	// 2. ���� ��Ʈ�� ����
	std::ofstream file(filePath, std::ios::binary);
	if (!file.is_open()) {
		return E_FAIL;
	}

	// 3. ��Ÿ�����Ϳ� ���ڿ� ���̸� ���
	metadata.stringLength = static_cast<uint32_t>(customString.length());

	// 4. ��Ÿ�����͸� ���Ͽ� ���
	file.write(reinterpret_cast<char*>(&metadata), sizeof(metadata));

	// 5. ���ڿ� �����͸� ���Ͽ� ���
	file.write(customString.c_str(), customString.length());

	// 6. WIC ��Ʈ�� ����
	hr = pWICFactory->CreateStream(&pStream);
	if (SUCCEEDED(hr)) {
		hr = pStream->InitializeFromFilename(filePath, GENERIC_WRITE);
	}

	if (SUCCEEDED(hr)) {
		// 7. PNG ���ڴ� ����
		hr = pWICFactory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &pEncoder);
	}

	if (SUCCEEDED(hr)) {
		hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
	}

	if (SUCCEEDED(hr)) {
		hr = pEncoder->CreateNewFrame(&pFrame, nullptr);
	}

	if (SUCCEEDED(hr)) {
		hr = pFrame->Initialize(nullptr);
	}

	if (SUCCEEDED(hr)) {
		// 8. ��Ʈ���� ũ�� ��������
		D2D1_SIZE_U size = pBitmap->GetPixelSize();
		hr = pFrame->SetSize(size.width, size.height);

		if (SUCCEEDED(hr)) {
			WICPixelFormatGUID format = GUID_WICPixelFormat32bppPBGRA;
			hr = pFrame->SetPixelFormat(&format);
		}

		if (SUCCEEDED(hr)) {
			D2D1_MAPPED_RECT mappedRect;
			hr = pBitmap->Map(D2D1_MAP_OPTIONS_READ, &mappedRect);

			if (SUCCEEDED(hr)) {
				hr = pFrame->WritePixels(
					size.height,
					mappedRect.pitch,
					mappedRect.pitch * size.height,
					mappedRect.bits
				);

				pBitmap->Unmap();
			}
		}

		if (SUCCEEDED(hr)) {
			hr = pFrame->Commit();
		}

		if (SUCCEEDED(hr)) {
			hr = pEncoder->Commit();
		}
	}

	// ��Ʈ���� ���ڴ� ����
	SAFE_RELEASE(pFrame);
	SAFE_RELEASE(pEncoder);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pWICFactory);

	// ���� ��Ʈ�� �ݱ�
	file.close();

	return hr;
}