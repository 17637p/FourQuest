#include "UIManager.h"

#include <d2d1.h>
#include <d2d1_1.h>
#include "d2d1helper.h"		// ColorF

#include <filesystem>

#include <dwrite.h>			// writefactory
#include <wincodec.h>

#include "RenderObject.h"

#include "D3D11Device.h"

#include <FQCommonGraphics.h>

fq::graphics::UIManager::UIManager()
	:mHWnd{ NULL },
	mDirect2DFactory{ nullptr },
	mRenderTarget{ nullptr },
	mBrushes{},
	mDWriteFactory{ nullptr },
	mFonts{},
	mDefaultFontPath{ L"Verdana" },
	mDefaultFontSize{ 50 },
	mDefaultFontColor{ 0, 1, 1, 1 },
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

	//mWICFactory->Release();
	mRenderTarget->Release();
	mDirect2DFactory->Release();
}

void fq::graphics::UIManager::Initialize(HWND hWnd, std::shared_ptr<D3D11Device> device, const short width, const short height)
{
	mHWnd = hWnd;

	HRESULT hr = S_OK;

	// 1. Direct2D Factory를 만든다.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mDirect2DFactory);

	// 렌더 타겟 초기화 
	createRenderTarget(device, width, height);

	initializeText();
	initializeImage();
}

HRESULT fq::graphics::UIManager::createRenderTarget(std::shared_ptr<D3D11Device> device, const short width, const short height)
{
	HRESULT hr = S_OK;

	D2D1_SIZE_U size = D2D1::SizeU(width, height);

	float dpi = GetDpiForWindow(mHWnd);

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			0,
			0);

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
	mRenderTarget->BeginDraw();

	drawAllText();
	drawAllImage();

	mRenderTarget->EndDraw();
}

void fq::graphics::UIManager::Finalize()
{

}

void fq::graphics::UIManager::AddFont(const std::wstring& path)
{
	IDWriteTextFormat* tempFont;

	HRESULT hr = S_OK;

	//Todo: Path 일 경우 regist 아니면 그냥 
	if (path.find(L".") != std::string::npos) // .가 하나라도 있으면 
	{
		registerFont(path);
		return;
	}

	// Create a DirectWrite text format object.
	// 1~100개의 폰트를 미리 만들어 둔다.
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

	// 텍스트Format 만들기 위해 필요한 WirteFactory
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
	createRenderTarget(device, width, height);
}

void fq::graphics::UIManager::registerFont(const std::wstring& path)
{
	mFontPath.emplace_back(path);

	auto a = AddFontResourceEx(path.c_str(), FR_NOT_ENUM, NULL);
	std::wstring filename = std::filesystem::path(path).filename();
	AddFont(filename.substr(0, filename.find_last_of(L".")));
}

void fq::graphics::UIManager::ReleaseRenderTarget()
{
	if (mRenderTarget != nullptr)
	{
		mRenderTarget->Release();
	}
}

void fq::graphics::UIManager::initializeImage()
{
	CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&mWICFactory));
}

void fq::graphics::UIManager::loadBitmap(const std::wstring& path)
{
	IWICBitmapDecoder* p_decoder = nullptr;
	HRESULT hr = mWICFactory->CreateDecoderFromFilename(path.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &p_decoder);

	IWICBitmapFrameDecode* p_frame;
	p_decoder->GetFrame(0, &p_frame);

	// iwicbitmap -> id2d1bitmap
	IWICFormatConverter* p_converter;
	mWICFactory->CreateFormatConverter(&p_converter);
	p_converter->Initialize(p_frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeCustom);

	ID2D1Bitmap* tempBitmap = nullptr;
	hr = mRenderTarget->CreateBitmapFromWicBitmap(p_converter, NULL, &tempBitmap);

	FQBitmap* fqBitmap = new FQBitmap;
	fqBitmap->bitmap = tempBitmap;
	fqBitmap->refCount = 1;
	mBitmaps[path] = fqBitmap;

	p_converter->Release();
	p_frame->Release();
	p_decoder->Release();
}

void fq::graphics::UIManager::AddImage(IImageObject* imageObject)
{
	mIsSorted = false;
	mImages.push_back(imageObject);
}

void fq::graphics::UIManager::DeleteImage(IImageObject* imageObject)
{
	std::filesystem::path stringToWstringPath = imageObject->GetImagePath();
	std::wstring imagePath = stringToWstringPath.wstring();

	mBitmaps[imagePath]->refCount--;
	if (mBitmaps[imagePath]->refCount == 0)
	{
		mBitmaps[imagePath]->bitmap->Release();
		delete mBitmaps[imagePath];
	}

	mImages.erase(remove(mImages.begin(), mImages.end(), imageObject), mImages.end());

	delete imageObject;
}

void fq::graphics::UIManager::drawAllText()
{
	if (mRenderTarget)
	{
		mRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		ID2D1SolidColorBrush* tempBrush = nullptr;
		for (const auto& drawTextInformation : mDrawTextInformations)
		{
			auto brush = mBrushes.find(drawTextInformation.color);
			if (brush == mBrushes.end())
			{
				// 브러시 제작 
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
	}
}

bool IImageObjectCmpLayer(fq::graphics::IImageObject* a, fq::graphics::IImageObject* b)
{
	return a->GetLayer() > b->GetLayer();
}

void fq::graphics::UIManager::drawAllImage()
{
	if (!mIsSorted)
	{
		// 레이어 단위로 정렬하기
		std::sort(mImages.begin(), mImages.end(), IImageObjectCmpLayer);
		//mImages
		mIsSorted = true;
	}

	for (const auto& image : mImages)
	{
		// 그리기
		std::filesystem::path stringToWstringPath = image->GetImagePath();
		std::wstring imagePath = stringToWstringPath.wstring();

		D2D1_SIZE_F imageSize = mBitmaps[imagePath]->bitmap->GetSize();
		D2D1_RECT_F imageRect = { 0, 0, imageSize.width * image->GetXRatio(), imageSize.height * image->GetYRatio() }; // 그릴 이미지(이미지 좌표) 따라서 비율은 여기서 결정 id2dbitmap 에 이미지의 사이즈를 가져올 수 있는 함수가 있음
		D2D1_RECT_F screenRect = { image->GetStartX(), image->GetStartY(), image->GetStartX() + image->GetWidth(), image->GetStartY() + image->GetHeight() }; // 그릴 크기 (화면 좌표)

		mRenderTarget->DrawBitmap(mBitmaps[imagePath]->bitmap, &screenRect, image->GetAlpha(), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &imageRect);
	}
}

fq::graphics::IImageObject* fq::graphics::UIManager::CreateImageObject(const UIInfo& uiInfo)
{
	IImageObject* newImageObject = new ImageObject;
	newImageObject->SetStartX(uiInfo.StartX);
	newImageObject->SetStartY(uiInfo.StartY);

	newImageObject->SetWidth(uiInfo.Width);
	newImageObject->SetHeight(uiInfo.Height);

	newImageObject->SetLayer(uiInfo.Layer);

	newImageObject->SetXRatio(uiInfo.XRatio);
	newImageObject->SetYRatio(uiInfo.YRatio);

	newImageObject->SetAlpha(uiInfo.Alpha);

	newImageObject->SetImagePath(uiInfo.ImagePath);

	// bitmap에서 찾은 다음에 없으면 만들 것
	std::filesystem::path stringToWstringPath = uiInfo.ImagePath;
	std::wstring imagePath = stringToWstringPath.wstring();

	auto bitmap = mBitmaps.find(imagePath);
	if (bitmap == mBitmaps.end())
	{
		loadBitmap(imagePath);
	}
	else
	{
		mBitmaps[imagePath]->refCount++;
	}

	AddImage(newImageObject);

	return newImageObject;
}
