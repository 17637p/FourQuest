#include "UIManager.h"

#include <d2d1.h>
#include <d2d1_1.h>
#include "d2d1helper.h"		// ColorF
#include <dwrite.h>			// writefactory
#include <filesystem>

#include "D3D11Device.h"

fq::graphics::UIManager::UIManager()
	:mHWnd{NULL},
	mDirect2DFactory{nullptr},
	mRenderTarget{nullptr},
	mBrushes{},
	mDWriteFactory{nullptr},
	mFonts{},
	mDefaultFontPath{L"Verdana"},
	mDefaultFontSize{50},
	mDefaultFontColor{0, 1, 1, 1},
	mDrawTextInformations{}
{

}

fq::graphics::UIManager::~UIManager()
{
	for (const auto& fontPath : mFontPath)
	{
		RemoveFontResourceEx(fontPath.c_str(), FR_PRIVATE, NULL);
	}

	for (const auto& brush : mBrushes)
	{
		brush.second->Release();
	}

	for (const auto& font : mFonts)
	{
		font.second->Release();
	}

	mRenderTarget->Release();
	mDirect2DFactory->Release();
}

void fq::graphics::UIManager::Initialize(HWND hWnd, std::shared_ptr<D3D11Device> device, const short width, const short height)
{
	mHWnd = hWnd;

	HRESULT hr = S_OK;

	// 1. Direct2D Factory�� �����.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mDirect2DFactory);

	// ���� Ÿ�� �ʱ�ȭ 
	createDeviceResources(device, width, height);

	initializeText();
}

HRESULT fq::graphics::UIManager::createDeviceResources(std::shared_ptr<D3D11Device> device, const short width, const short height)
{
	HRESULT hr = S_OK;

	if (mRenderTarget != nullptr)
	{
		mRenderTarget->Release();
	}
	// 2. Create a Direct2D render target.
	//RECT rc;
	//GetClientRect(mHWnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(width, height);

	float dpi = GetDpiForWindow(mHWnd);

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			0,
			0);

	//hr = mDirect2DFactory->CreateHwndRenderTarget(
	//	D2D1::RenderTargetProperties(),
	//	D2D1::HwndRenderTargetProperties(mHWnd, size),
	//	&mRenderTarget);

	IDXGISurface* backBuffer;
	device->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	hr = mDirect2DFactory->CreateDxgiSurfaceRenderTarget(
		backBuffer,
		&props,
		&mRenderTarget);

	backBuffer->Release();
	
	return hr;
}

void fq::graphics::UIManager::Render()
{
	if (mRenderTarget)
	{
		//D2D1_SIZE_F renderTargetSize = mRenderTarget->GetSize();
		mRenderTarget->BeginDraw();
		mRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		//mRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		ID2D1SolidColorBrush* tempBrush = nullptr;
		for (const auto& drawTextInformation : mDrawTextInformations)
		{
			auto brush = mBrushes.find(drawTextInformation.color);
			if (brush == mBrushes.end())
			{
				// �귯�� ���� 
				// Create a black brush.
				mRenderTarget->CreateSolidColorBrush(
					D2D1::ColorF(drawTextInformation.color.R(),
						drawTextInformation.color.G(),
						drawTextInformation.color.B(),
						drawTextInformation.color.A()),
					&tempBrush);

				mBrushes[drawTextInformation.color] = tempBrush;
			}

			mRenderTarget->DrawText(
				drawTextInformation.text.c_str(),
				//ARRAYSIZE(drawTextInformation.text.c_str()) - 1,
				drawTextInformation.text.length(),
				mFonts[drawTextInformation.fontPath + std::to_wstring(drawTextInformation.fontSize)],
				D2D1::RectF(
					drawTextInformation.drawRect.x - drawTextInformation.drawRect.width / 2,
					drawTextInformation.drawRect.y - drawTextInformation.drawRect.height / 2,
					drawTextInformation.drawRect.x + drawTextInformation.drawRect.width / 2,
					drawTextInformation.drawRect.y + drawTextInformation.drawRect.height / 2),
				mBrushes[drawTextInformation.color]);
		}
		mDrawTextInformations.clear();

		mRenderTarget->EndDraw();
	}
}

void fq::graphics::UIManager::Finalize()
{
	
}

void fq::graphics::UIManager::AddFont(const std::wstring& path)
{
	IDWriteTextFormat* tempFont;

	HRESULT hr = S_OK;

	//Todo: Path �� ��� regist �ƴϸ� �׳� 
	if (path.find(L".") != std::string::npos) // .�� �ϳ��� ������ 
	{
		RegisterFont(path);
		return;
	}

	// Create a DirectWrite text format object.
	// 1~100���� ��Ʈ�� �̸� ����� �д�.
	for (unsigned short fontSize = 1; fontSize <= 100; fontSize++)
	{
		hr = mDWriteFactory->CreateTextFormat(
			path.c_str(),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"", //locale
			&tempFont
		);

		mFonts[path + std::to_wstring(fontSize)] = tempFont;
	}
}

void fq::graphics::UIManager::initializeText()
{
	HRESULT hr = S_OK;

	// �ؽ�ƮFormat ����� ���� �ʿ��� WirteFactory
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(mDWriteFactory),
			reinterpret_cast<IUnknown**>(&mDWriteFactory)
		);
	}

	AddFont(L"Verdana");
	SetDefaultFont(L"Verdana");
	SetDefaultFontSize(50);
	SetDefaultFontColor({ 0,0,1,1 });
}

void fq::graphics::UIManager::SetDefaultFont(const std::wstring& fontPath)
{
	mDefaultFontPath = fontPath;
}

void fq::graphics::UIManager::SetDefaultFontSize(const unsigned short fontSize)
{
	mDefaultFontSize = fontSize;
}

void fq::graphics::UIManager::SetDefaultFontColor(const DirectX::SimpleMath::Color& color)
{
	mDefaultFontColor = color;
}

void fq::graphics::UIManager::DeleteFont(const std::wstring& path)
{
	for (unsigned short fontSize = 1; fontSize <= 100; fontSize++)
	{
		mFonts.erase(path + std::to_wstring(fontSize));
	}
}

void fq::graphics::UIManager::DrawText(const std::wstring& text, const DirectX::SimpleMath::Rectangle& drawRect, unsigned short fontSize /*= 50*/, const std::wstring& fontPath /*= L"Verdana"*/, const DirectX::SimpleMath::Color& color /*= {1, 0, 0, 1}*/)
{
	DrawTextInformation drawTextInformation;

	drawTextInformation.text = text;
	drawTextInformation.drawRect = drawRect;

	if (fontSize == 50)
	{
		drawTextInformation.fontSize = mDefaultFontSize;
	}
	else
	{
		drawTextInformation.fontSize = fontSize;
	}

	if (fontPath == L"Verdana")
	{
		drawTextInformation.fontPath = mDefaultFontPath;
	}
	else
	{
		drawTextInformation.fontPath = fontPath;
	}

	if (color == DirectX::SimpleMath::Color{ 0, 1, 1, 1 })
	{
		drawTextInformation.color = mDefaultFontColor;
	}
	else
	{
		drawTextInformation.color = color;
	}

	mDrawTextInformations.emplace_back(drawTextInformation);
}

void fq::graphics::UIManager::OnResize(std::shared_ptr<D3D11Device> device, const short width, const short height)
{
	createDeviceResources(device, width, height);
}

void fq::graphics::UIManager::RegisterFont(const std::wstring& path)
{
	mFontPath.emplace_back(path);

	auto a =  AddFontResourceEx(path.c_str(), FR_NOT_ENUM, NULL);
	std::wstring filename = std::filesystem::path(path).filename();
	AddFont(filename.substr(0, filename.find_last_of(L".")));
}
