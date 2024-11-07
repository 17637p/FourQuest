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

#include "D3D11ResourceManager.h"
#include "D3D11View.h"

#include <boost/locale.hpp>

#include <spdlog/spdlog.h>

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
	mTexts{},
	mResourceManager(nullptr),
	mIsRenderObjects(true)
{
	boost::locale::generator gen;
	std::locale loc = gen("en_US.UTF-8");
	std::locale::global(loc);
}

fq::graphics::UIManager::~UIManager()
{
	if (pSourceReader != NULL)
	{
		pSourceReader->Release();
	}
	if (mVideoBitmap != nullptr)
	{
		mVideoBitmap->Release();
	}
	if (mVideoObject != nullptr)
	{
		delete mVideoObject;
	}

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

void fq::graphics::UIManager::Initialize(HWND hWnd, std::shared_ptr<D3D11Device> device,
	std::shared_ptr<D3D11ResourceManager> resourceManager,
	const short width, const short height)
{
	mHWnd = hWnd;
	mResourceManager = resourceManager;

	HRESULT hr = S_OK;

	// 1. Direct2D Factory를 만든다.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mDirect2DFactory);

	// 렌더 타겟 초기화 
	createRenderTarget(device, width, height);

	initializeText();
	initializeImage();

	MFStartup(MF_VERSION);
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

	//IDXGISurface* backBuffer;
	//device->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	IDXGISurface1* pDXGISurface = nullptr;
	auto renderTarget = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);

	ID3D11Texture2D* texture = nullptr;
	renderTarget->GetRTV().Get()->GetResource(reinterpret_cast<ID3D11Resource**>(&texture));

	texture->QueryInterface<IDXGISurface1>(&pDXGISurface);

	//hr = renderTarget->GetRTV().Get()->
		//->QueryInterface(__uuidof(IDXGISurface), (void**)&pDXGISurface);

	hr = mDirect2DFactory->CreateDxgiSurfaceRenderTarget(
		pDXGISurface,
		&props,
		&mRenderTarget);

	//backBuffer->Release();
	pDXGISurface->Release();
	texture->Release();

	return hr;
}

void fq::graphics::UIManager::Render()
{
	mRenderTarget->BeginDraw();

	if (!mIsRenderObjects)
	{
		mRenderTarget->Clear(D2D1_COLOR_F{ 0, 0, 0, 1 });
	}

	draw();

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
	for (unsigned short fontSize = 1; fontSize <= 150; fontSize++)
	{
		hr = mDWriteFactory->CreateTextFormat(
			path.c_str(),
			//L"KBIZmjo",
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

	// DWrite 초기화
	//IDWriteFactory* pDWriteFactory = nullptr;
	//DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
	//
	//IDWriteFontCollection* pFontCollection = nullptr;
	//pDWriteFactory->GetSystemFontCollection(&pFontCollection);
	//
	//UINT32 familyCount = pFontCollection->GetFontFamilyCount();
	//for (UINT32 i = 0; i < familyCount; ++i) {
	//	IDWriteFontFamily* pFontFamily = nullptr;
	//	pFontCollection->GetFontFamily(i, &pFontFamily);
	//
	//	IDWriteLocalizedStrings* pFamilyNames = nullptr;
	//	pFontFamily->GetFamilyNames(&pFamilyNames);
	//
	//	UINT32 index = 0;
	//	BOOL exists = false;
	//	wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
	//	if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH)) {
	//		pFamilyNames->FindLocaleName(localeName, &index, &exists);
	//	}
	//
	//	if (!exists) index = 0;
	//
	//	UINT32 length = 0;
	//	pFamilyNames->GetStringLength(index, &length);
	//	wchar_t* name = new (std::nothrow) wchar_t[length + 1];
	//	if (name) {
	//		pFamilyNames->GetString(index, name, length + 1);
	//		wprintf(L"Font Family: %s\n", name);
	//		delete[] name;
	//	}
	//	pFamilyNames->Release();
	//	pFontFamily->Release();
	//}
	//pFontCollection->Release();
	//pDWriteFactory->Release();
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
	RemoveFontResourceEx(path.c_str(), FR_NOT_ENUM, NULL);
}

fq::graphics::ITextObject* fq::graphics::UIManager::CreateText(TextInfo textInfo)
{
	mIsSortedText = false;

	TextObject* newTextObject = new TextObject;
	newTextObject->SetTextInformation(textInfo);

	mTexts.emplace_back(newTextObject);

	return newTextObject;
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
	mIsSortedImage = false;
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
		mBitmaps.erase(imagePath);
	}

	// mask 도
	stringToWstringPath = imageObject->GetMaskPath();
	std::wstring maskPath = stringToWstringPath.wstring();

	if (maskPath != L"")
	{
		mBitmaps[maskPath]->refCount--;
		if (mBitmaps[maskPath]->refCount == 0)
		{
			mBitmaps[maskPath]->bitmap->Release();
			delete mBitmaps[maskPath];
			mBitmaps.erase(maskPath);
		}
	}

	mImages.erase(remove(mImages.begin(), mImages.end(), imageObject), mImages.end());

	delete imageObject;
}

bool IImageObjectCmpLayer(fq::graphics::IImageObject* a, fq::graphics::IImageObject* b)
{
	return a->GetLayer() > b->GetLayer();
}

bool ITextObjectCmpLayer(fq::graphics::ITextObject* a, fq::graphics::ITextObject* b)
{
	return a->GetTextInformation().Layer > b->GetTextInformation().Layer;
}

bool ISpriteAnimationObjectCmpLayer(fq::graphics::ISpriteAnimationObject* a, fq::graphics::ISpriteAnimationObject* b)
{
	return a->GetSpriteInfo().Layer > b->GetSpriteInfo().Layer;
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
	newImageObject->SetMaskPath(uiInfo.MaskPath);

	newImageObject->SetScaleX(uiInfo.ScaleX);
	newImageObject->SetScaleY(uiInfo.ScaleY);
	newImageObject->SetRotation(uiInfo.RotationAngle);

	newImageObject->SetFillDegree(uiInfo.fillDegree);
	newImageObject->SetRenderMode(uiInfo.isCenter);

	newImageObject->SetColor(uiInfo.Color);
	newImageObject->SetIsRender(uiInfo.isRender);

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

	// mask도
	if (uiInfo.MaskPath != "")
	{
		stringToWstringPath = uiInfo.MaskPath;
		imagePath = stringToWstringPath.wstring();

		auto maskBitmap = mBitmaps.find(imagePath);
		if (maskBitmap == mBitmaps.end())
		{
			loadBitmap(imagePath);
		}
		else
		{
			mBitmaps[imagePath]->refCount++;
		}
	}

	AddImage(newImageObject);

	return newImageObject;
}

void fq::graphics::UIManager::DeleteText(fq::graphics::ITextObject* textObject)
{
	mTexts.erase(std::remove(mTexts.begin(), mTexts.end(), textObject), mTexts.end());
	delete textObject;
}

std::wstring fq::graphics::UIManager::stringToWstring(std::string str)
{
	// 문자열 변환
	std::string narrow_str = str;
	std::wstring wide_str = boost::locale::conv::to_utf<wchar_t>(narrow_str, "UTF-8");

	return wide_str;
}

void fq::graphics::UIManager::draw()
{
	// 이미지 정렬
	if (!mIsSortedImage)
	{
		std::sort(mImages.begin(), mImages.end(), IImageObjectCmpLayer);
		mIsSortedImage = true;
	}
	// 텍스트 정렬
	if (!mIsSortedText)
	{
		std::sort(mTexts.begin(), mTexts.end(), ITextObjectCmpLayer);
		mIsSortedText = true;
	}
	// 애니메이션 정렬
	if (!mIsSortedSpriteAnimation)
	{
		std::sort(mSpriteAnimations.begin(), mSpriteAnimations.end(), ISpriteAnimationObjectCmpLayer);
		mIsSortedSpriteAnimation = true;
	}

	int imageIndex = 0;
	int textIndex = 0;
	int spriteAnimationIndex = 0;
	while (true)
	{
		if (imageIndex >= mImages.size())
		{
			imageIndex = -1;
		}
		if (textIndex >= mTexts.size())
		{
			textIndex = -1;
		}
		if (spriteAnimationIndex >= mSpriteAnimations.size())
		{
			spriteAnimationIndex = -1;
		}

		if (imageIndex == -1 && textIndex == -1 && spriteAnimationIndex == -1)
		{
			break;
		}

		if (imageIndex == -1)
		{
			if (textIndex == -1)
			{
				drawSpriteAnimation(mSpriteAnimations[spriteAnimationIndex]);
				spriteAnimationIndex++;
			}
			else if (spriteAnimationIndex == -1)
			{
				drawText(mTexts[textIndex]);
				textIndex++;
			}
			else
			{
				if (mSpriteAnimations[spriteAnimationIndex]->GetSpriteInfo().Layer > mTexts[textIndex]->GetTextInformation().Layer)
				{
					drawSpriteAnimation(mSpriteAnimations[spriteAnimationIndex]);
					spriteAnimationIndex++;
				}
				else
				{
					drawText(mTexts[textIndex]);
					textIndex++;
				}
			}
		}
		else if (textIndex == -1)
		{
			if (spriteAnimationIndex == -1)
			{
				drawImage(mImages[imageIndex]);
				imageIndex++;
			}
			else if (imageIndex == -1)
			{
				drawSpriteAnimation(mSpriteAnimations[spriteAnimationIndex]);
				spriteAnimationIndex++;
			}
			else
			{
				if (mSpriteAnimations[spriteAnimationIndex]->GetSpriteInfo().Layer > mImages[imageIndex]->GetLayer())
				{
					drawSpriteAnimation(mSpriteAnimations[spriteAnimationIndex]);
					spriteAnimationIndex++;
				}
				else
				{
					drawImage(mImages[imageIndex]);
					imageIndex++;
				}
			}
		}
		else if (spriteAnimationIndex == -1)
		{
			if (textIndex == -1)
			{
				drawImage(mImages[imageIndex]);
				imageIndex++;
			}
			else if (imageIndex == -1)
			{
				drawText(mTexts[textIndex]);
				textIndex++;
			}
			else
			{
				if (mTexts[textIndex]->GetTextInformation().Layer > mImages[imageIndex]->GetLayer())
				{
					drawText(mTexts[textIndex]);
					textIndex++;
				}
				else
				{
					drawImage(mImages[imageIndex]);
					imageIndex++;
				}
			}
		}

		else if (mImages[imageIndex]->GetLayer() > mTexts[textIndex]->GetTextInformation().Layer
			&& mImages[imageIndex]->GetLayer() > mSpriteAnimations[spriteAnimationIndex]->GetSpriteInfo().Layer)
		{
			drawImage(mImages[imageIndex]);
			imageIndex++;
		}
		else if (mTexts[textIndex]->GetTextInformation().Layer > mSpriteAnimations[spriteAnimationIndex]->GetSpriteInfo().Layer)
		{
			drawText(mTexts[textIndex]);
			textIndex++;
		}
		else
		{
			drawSpriteAnimation(mSpriteAnimations[spriteAnimationIndex]);
			spriteAnimationIndex++;
		}
	}

	if (mVideoObject != nullptr && mVideoBitmap != nullptr)
	{
		VideoInfo videoInfo = mVideoObject->GetVideoInfo();
		if (videoInfo.isRender)
		{
			mRenderTarget->SetTransform
			(
				D2D1::Matrix3x2F::Scale(videoInfo.ScaleX, videoInfo.ScaleY, D2D1::Point2F(videoInfo.StartX, videoInfo.StartY))
			);
			mRenderTarget->DrawBitmap(mVideoBitmap, D2D1::RectF(videoInfo.StartX, videoInfo.StartY, videoInfo.StartX + mVideoObject->GetVideoInfo().Width, videoInfo.StartY + mVideoObject->GetVideoInfo().Height));
		}
	}
}

void fq::graphics::UIManager::drawText(fq::graphics::ITextObject* textObject)
{
	if (mRenderTarget)
	{
		mRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		ID2D1SolidColorBrush* tempBrush = nullptr;


		if (!textObject->GetTextInformation().IsRender)
		{
			return;
		}

		TextInfo drawTextInformation = textObject->GetTextInformation();
		if (drawTextInformation.Text == "")
		{
			return;
		}

		if (!drawTextInformation.IsRender)
		{
			return;
		}

		DirectX::SimpleMath::Vector2 size{0, 0};
		if (drawTextInformation.isUseAutoCenterAlign)
		{
			IDWriteTextLayout* pTextLayout = nullptr;
			std::wstring text = stringToWstring(drawTextInformation.Text);
			std::wstring fontPath = stringToWstring(drawTextInformation.FontPath) + std::to_wstring(drawTextInformation.FontSize);

			if (mFonts.find(fontPath) == mFonts.end())
			{
				fontPath = L"Verdana" + std::to_wstring(drawTextInformation.FontSize);
			}
			HRESULT hr = mDWriteFactory->CreateTextLayout(
				text.c_str(),            // 텍스트
				text.length(),    // 텍스트 길이
				mFonts[fontPath],     // 텍스트 포맷
				FLT_MAX,         // 최대 폭 (조정 가능)
				FLT_MAX,         // 최대 높이 (조정 가능)
				&pTextLayout
			);

			if (SUCCEEDED(hr))
			{
				// 텍스트 레이아웃의 크기를 가져옵니다.
				DWRITE_TEXT_METRICS textMetrics;
				hr = pTextLayout->GetMetrics(&textMetrics);
				if (SUCCEEDED(hr))
				{
					size.x = textMetrics.width;
					size.y = textMetrics.height;
				}
				pTextLayout->Release();
			}

			drawTextInformation.CenterX -= size.x / 2;
		}

		auto brush = mBrushes.find(drawTextInformation.FontColor);
		if (brush == mBrushes.end())
		{
			// 브러시 제작 
			// Create a black brush.
			mRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(drawTextInformation.FontColor.R(),
					drawTextInformation.FontColor.G(),
					drawTextInformation.FontColor.B(),
					drawTextInformation.FontColor.A()),
				&tempBrush);

			mBrushes[drawTextInformation.FontColor] = tempBrush;
		}

		std::wstring text = stringToWstring(drawTextInformation.Text);

		// \\n 찾아서 \n으로 바꾸기
		std::wstring toReplace = L"\\n";
		std::wstring replaceWith = L"\n";

		size_t pos = text.find(toReplace);

		while (pos != std::wstring::npos)
		{
			text.replace(pos, toReplace.length(), replaceWith);
			pos = text.find(toReplace, pos + replaceWith.length());
		}

		std::wstring fontPath = stringToWstring(drawTextInformation.FontPath) + std::to_wstring(drawTextInformation.FontSize);

		if (mFonts.find(fontPath) == mFonts.end())
		{
			fontPath = L"Verdana" + std::to_wstring(drawTextInformation.FontSize);
		}

		switch (textObject->GetTextInformation().Align)
		{
			case ETextAlign::LeftTop:
			{
				mFonts[fontPath]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				mFonts[fontPath]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
				break;
			}
			case ETextAlign::LeftCenter:
			{
				mFonts[fontPath]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				mFonts[fontPath]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				break;
			}
			case ETextAlign::LeftBottom:
			{
				mFonts[fontPath]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				mFonts[fontPath]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
				break;
			}
			case ETextAlign::CenterTop:
			{
				mFonts[fontPath]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				mFonts[fontPath]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
				break;
			}
			case ETextAlign::CenterCenter:
			{
				mFonts[fontPath]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				mFonts[fontPath]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				break;
			}
			case ETextAlign::CenterBottom:
			{
				mFonts[fontPath]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				mFonts[fontPath]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
				break;
			}
			case ETextAlign::RightTop:
			{
				mFonts[fontPath]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
				mFonts[fontPath]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
				break;
			}
			case ETextAlign::RightCenter:
			{
				mFonts[fontPath]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
				mFonts[fontPath]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				break;
			}
			case ETextAlign::RightBottom:
			{
				mFonts[fontPath]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
				mFonts[fontPath]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
				break;
			}
			default:
				break;
		}

		float scaleX = drawTextInformation.ScaleX;
		float scaleY = drawTextInformation.ScaleY;
		mRenderTarget->SetTransform
		(
			D2D1::Matrix3x2F::Scale(scaleX, scaleY, D2D1::Point2F(drawTextInformation.CenterX, drawTextInformation.CenterY))
		);

		if (textObject->GetTextInformation().IsUseSetSpacing)
		{
			mFonts[fontPath]->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, 50, 80);
		}

		if (textObject->GetTextInformation().BoxAlign == ETextBoxAlign::CenterCenter)
		{
			mRenderTarget->DrawText(
				text.c_str(),
				text.length(),
				mFonts[fontPath],
				D2D1::RectF(
					drawTextInformation.CenterX - drawTextInformation.Width / 2,
					drawTextInformation.CenterY - drawTextInformation.Height / 2,
					drawTextInformation.CenterX + drawTextInformation.Width / 2,
					drawTextInformation.CenterY + drawTextInformation.Height / 2),
				mBrushes[drawTextInformation.FontColor]);
		}
		else
		{
			mRenderTarget->DrawText(
				text.c_str(),
				text.length(),
				mFonts[fontPath],
				D2D1::RectF(
					drawTextInformation.CenterX,
					drawTextInformation.CenterY,
					drawTextInformation.CenterX + drawTextInformation.Width,
					drawTextInformation.CenterY + drawTextInformation.Height),
				mBrushes[drawTextInformation.FontColor]);
		}

	}
}

void fq::graphics::UIManager::drawImage(IImageObject* image)
{
	if (!image->GetIsRender())
	{
		return;
	}

	// 그리기
	std::filesystem::path stringToWstringPath = image->GetImagePath();
	std::wstring imagePath = stringToWstringPath.wstring();

	D2D1_SIZE_F imageSize = mBitmaps[imagePath]->bitmap->GetSize();
	D2D1_RECT_F imageRect = { 0, 0, imageSize.width * image->GetXRatio(), imageSize.height * image->GetYRatio() }; // 그릴 이미지(이미지 좌표) 따라서 비율은 여기서 결정 id2dbitmap 에 이미지의 사이즈를 가져올 수 있는 함수가 있음
	D2D1_RECT_F screenRect{};
	if (image->GetRenderMode()) // true가 isCenter
	{
		screenRect = { image->GetStartX() - image->GetWidth() / 2, image->GetStartY() - image->GetHeight() / 2,
		image->GetStartX() + image->GetWidth() / 2, image->GetStartY() + image->GetHeight() / 2 }; // 그릴 크기 (화면 좌표)
	}
	else
	{
		screenRect = { image->GetStartX(), image->GetStartY(),
		image->GetStartX() + image->GetWidth(), image->GetStartY() + image->GetHeight() }; // 그릴 크기 (화면 좌표)
	}

	if (image->GetColor() != DirectX::SimpleMath::Color(0, 0, 0, 1))
	{
		ID2D1SolidColorBrush* brush;
		D2D1_COLOR_F d2dColor;
		DirectX::SimpleMath::Color color = image->GetColor();
		d2dColor.r = color.R();
		d2dColor.g = color.G();
		d2dColor.b = color.B();
		d2dColor.a = color.A();

		mRenderTarget->SetTransform
		(
			D2D1::Matrix3x2F::Rotation(image->GetRotation(), D2D1::Point2F(image->GetStartX() + image->GetWidth() / 2, image->GetStartY() + image->GetHeight() / 2))
			* D2D1::Matrix3x2F::Scale(image->GetScaleX(), image->GetScaleY(), D2D1::Point2F(image->GetStartX(), image->GetStartY()))
		);

		mRenderTarget->CreateSolidColorBrush(d2dColor, &brush);
		mRenderTarget->FillRectangle(screenRect, brush);
		brush->Release();

		return;
	}

	//  마스크 있을 때
	if (image->GetMaskPath() != "")
	{
		stringToWstringPath = image->GetMaskPath();
		std::wstring maskPath = stringToWstringPath.wstring();

		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp =
			D2D1::BitmapBrushProperties(
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
			);

		ID2D1BitmapBrush* bitmapBrush;

		mRenderTarget->CreateBitmapBrush(
			mBitmaps[imagePath]->bitmap,
			propertiesXClampYClamp,
			&bitmapBrush
		);

		D2D_SIZE_U bitmapSize = mBitmaps[imagePath]->bitmap->GetPixelSize();
		float scaleX = image->GetWidth() / bitmapSize.width;
		float scaleY = image->GetHeight() / bitmapSize.height;

		D2D1_MATRIX_3X2_F scaleT = D2D1::Matrix3x2F::Scale(scaleX, scaleY);
		D2D1_MATRIX_3X2_F translateT = D2D1::Matrix3x2F::Translation(image->GetStartX(), image->GetStartY());

		D2D1_MATRIX_3X2_F totalT = D2D1::Matrix3x2F::Translation(image->GetStartX(), image->GetStartY());

		bitmapBrush->SetTransform(scaleT * translateT);

		mRenderTarget->SetTransform
		(
			D2D1::Matrix3x2F::Rotation(image->GetRotation(), D2D1::Point2F(image->GetStartX() + image->GetWidth() / 2, image->GetStartY() + image->GetHeight() / 2))
			* D2D1::Matrix3x2F::Scale(image->GetScaleX(), image->GetScaleY(), D2D1::Point2F(image->GetStartX(), image->GetStartY()))
		);

		// D2D1_ANTIALIAS_MODE_ALIASED must be set for FillOpacityMask to function properly
		mRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		mRenderTarget->FillOpacityMask(
			mBitmaps[maskPath]->bitmap,
			bitmapBrush,
			D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
			&screenRect,
			&imageRect
		);
		mRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		bitmapBrush->Release();
	}
	else
	{
		float degree = image->GetFillDegree();
		if (degree >= 0)
		{
			float radian = DirectX::XMConvertToRadians(degree - 270);

			float radiusX = (image->GetWidth() / 2);
			float radiusY = (image->GetHeight() / 2);

			float startX = image->GetStartX() - radiusX;
			float startY = image->GetStartY() - radiusY;

			// 경로 기하학을 생성하여 반원 모양을 정의합니다.
			ID2D1PathGeometry* pPathGeometry = nullptr;

			mDirect2DFactory->CreatePathGeometry(&pPathGeometry);
			ID2D1GeometrySink* pSink = nullptr;
			pPathGeometry->Open(&pSink);

			float cos = std::cosf(radian) * radiusX;
			float sin = std::sinf(radian) * radiusY;

			D2D1_POINT_2F BeginPoint = D2D1::Point2F(startX + radiusX, startY + radiusY + radiusY);
			pSink->BeginFigure(BeginPoint, D2D1_FIGURE_BEGIN_FILLED);

			D2D1_ARC_SIZE arcSize = D2D1_ARC_SIZE_SMALL;
			if (degree >= 180)
			{
				arcSize = D2D1_ARC_SIZE_LARGE;
			}

			pSink->AddArc(D2D1::ArcSegment(
				D2D1::Point2F(startX + radiusX + cos, startY + radiusY + sin),
				D2D1::SizeF(radiusX, radiusY),
				0.f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				arcSize
			));

			pSink->AddLine(D2D1::Point2F(startX + radiusX, startY + radiusY)); // 원의 끝 점에서 수직 하단으로 선을 추가
			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

			pSink->Close();
			pSink->Release();

			mRenderTarget->SetTransform
			(
				D2D1::Matrix3x2F::Rotation(image->GetRotation(), D2D1::Point2F(image->GetStartX() + image->GetWidth() / 2, image->GetStartY() + image->GetHeight() / 2))
				* D2D1::Matrix3x2F::Scale(image->GetScaleX(), image->GetScaleY(), D2D1::Point2F(image->GetStartX(), image->GetStartY()))
			);

			// 클리핑 영역을 설정합니다.
			mRenderTarget->PushLayer(
				D2D1::LayerParameters(
					D2D1::InfiniteRect(),
					pPathGeometry
				),
				nullptr
			);

			// 이미지를 그립니다.
			mRenderTarget->DrawBitmap(mBitmaps[imagePath]->bitmap, &screenRect, image->GetAlpha(), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &imageRect);

			// 클리핑 영역을 해제합니다.
			mRenderTarget->PopLayer();

			// 리소스를 해제합니다.
			pPathGeometry->Release();
		}
		else
		{
			// 왜 스케일 위치가 중앙점이 아니라 시작점이지...?
			mRenderTarget->SetTransform
			(
				D2D1::Matrix3x2F::Rotation(image->GetRotation(), D2D1::Point2F(image->GetStartX() + image->GetWidth() / 2, image->GetStartY() + image->GetHeight() / 2))
				* D2D1::Matrix3x2F::Scale(image->GetScaleX(), image->GetScaleY(), D2D1::Point2F(image->GetStartX(), image->GetStartY()))
			);

			mRenderTarget->DrawBitmap(mBitmaps[imagePath]->bitmap, &screenRect, image->GetAlpha(), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &imageRect);
		}
	}

}

fq::graphics::ISpriteAnimationObject* fq::graphics::UIManager::CreateSpriteAniamtion(SpriteInfo spriteInfo)
{
	mIsSortedSpriteAnimation = false;

	std::filesystem::path stringToWstringPath = spriteInfo.ImagePath;
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

	SpriteAnimationObject* newSpriteAnimationObject = new SpriteAnimationObject;
	newSpriteAnimationObject->SetSpriteInfo(spriteInfo);

	mSpriteAnimations.emplace_back(newSpriteAnimationObject);

	return newSpriteAnimationObject;
}

void fq::graphics::UIManager::DeleteSpriteAniamtion(ISpriteAnimationObject* spriteAniamtionObject)
{
	std::filesystem::path stringToWstringPath = spriteAniamtionObject->GetSpriteInfo().ImagePath;
	std::wstring imagePath = stringToWstringPath.wstring();

	mBitmaps[imagePath]->refCount--;
	if (mBitmaps[imagePath]->refCount == 0)
	{
		mBitmaps[imagePath]->bitmap->Release();
		delete mBitmaps[imagePath];
		mBitmaps.erase(imagePath);
	}

	mSpriteAnimations.erase(std::remove(mSpriteAnimations.begin(), mSpriteAnimations.end(), spriteAniamtionObject), mSpriteAnimations.end());
	delete spriteAniamtionObject;
}

void fq::graphics::UIManager::drawSpriteAnimation(ISpriteAnimationObject* spriteAnimationObject)
{
	SpriteInfo spriteInfo = spriteAnimationObject->GetSpriteInfo();
	if (!spriteInfo.isRender)
	{
		return;
	}

	// 그리기
	std::filesystem::path stringToWstringPath = spriteInfo.ImagePath;
	std::wstring imagePath = stringToWstringPath.wstring();

	//D2D1_SIZE_F imageSize = mBitmaps[imagePath]->bitmap->GetSize();
	D2D1_SIZE_F imageSize;
	imageSize.width = spriteInfo.Width;
	imageSize.height = spriteInfo.Height;
	float animWidth = imageSize.width / (float)spriteInfo.XNum;
	float animHeight = imageSize.height / (float)spriteInfo.YNum;
	D2D1_RECT_F imageRect = { 
		animWidth * (spriteInfo.CurImage % spriteInfo.XNum), 
		animHeight * (spriteInfo.CurImage / spriteInfo.XNum),
		animWidth * (spriteInfo.CurImage % spriteInfo.XNum) + animWidth,
		animHeight * (spriteInfo.CurImage / spriteInfo.XNum) + animHeight }; // 그릴 이미지(이미지 좌표) 따라서 비율은 여기서 결정 id2dbitmap 에 이미지의 사이즈를 가져올 수 있는 함수가 있음
	D2D1_RECT_F screenRect{};
	if (spriteInfo.isCenter) // true가 isCenter
	{
		screenRect = { spriteInfo.StartX - animWidth / 2, spriteInfo.StartY - animHeight / 2,
		spriteInfo.StartX + animWidth / 2, spriteInfo.StartY + animHeight / 2 }; // 그릴 크기 (화면 좌표)
	}
	else
	{
		screenRect = { spriteInfo.StartX, spriteInfo.StartY,
		spriteInfo.StartX + animWidth, spriteInfo.StartY + animHeight }; // 그릴 크기 (화면 좌표)
	}

	// 왜 스케일 위치가 중앙점이 아니라 시작점이지...?
	mRenderTarget->SetTransform
	(
		D2D1::Matrix3x2F::Scale(spriteInfo.ScaleX, spriteInfo.ScaleY, D2D1::Point2F(spriteInfo.StartX, spriteInfo.StartY))
	);

	mRenderTarget->DrawBitmap(mBitmaps[imagePath]->bitmap, &screenRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &imageRect);
}

bool fq::graphics::UIManager::LoadVideo(const std::wstring& path)
{
	IMFSourceResolver* pSourceResolver = NULL;
	MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;
	IUnknown* uSource = NULL;
	IMFMediaSource* mediaFileSource = NULL;
	IMFAttributes* pVideoReaderAttributes = NULL;
	IMFMediaType* pReaderOutputType = NULL;
	IMFMediaType* pFirstOutputType = NULL;

	MFCreateSourceResolver(&pSourceResolver);
	pSourceResolver->CreateObjectFromURL(
		path.c_str(),		        // URL of the source.
		MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
		NULL,                       // Optional property store.
		&ObjectType,				        // Receives the created object type. 
		&uSource					          // Receives a pointer to the media source.
	);

	uSource->QueryInterface(IID_PPV_ARGS(&mediaFileSource));

	MFCreateAttributes(&pVideoReaderAttributes, 2);

	pVideoReaderAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	pVideoReaderAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1);
	pVideoReaderAttributes->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);

	MFCreateSourceReaderFromMediaSource(mediaFileSource, pVideoReaderAttributes, &pSourceReader);

	MFCreateMediaType(&pReaderOutputType);
	pReaderOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	pReaderOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);

	pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pReaderOutputType);

	IMFMediaType* pNativeType = nullptr;
	pSourceReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, MF_SOURCE_READER_CURRENT_TYPE_INDEX, &pNativeType);
	pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pFirstOutputType);

	// The sample uses the pNativeType here, but we want the resolution required to 'render' the texture as if the height is not x16 the conversion
	// YUV to RGB has problems
	MFGetAttributeSize(pFirstOutputType, MF_MT_FRAME_SIZE, &mVideoWidth, &mVideoHeight);

	pSourceReader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
	pSourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);

	pFirstOutputType->Release();
	pReaderOutputType->Release();
	pVideoReaderAttributes->Release();
	mediaFileSource->Release();
	uSource->Release();
	pSourceResolver->Release();
	return true;
}

void fq::graphics::UIManager::UpdateVideoBitmap()
{
	if (mVideoObject == nullptr)
	{
		return;
	}
	VideoInfo videoInfo = mVideoObject->GetVideoInfo();
	if(!videoInfo.isRender)
	{
		return;
	}
	if (videoInfo.isReset)
	{
		PROPVARIANT var;

		var.vt = VT_I8;
		var.hVal.QuadPart = 0;

		HRESULT hr = pSourceReader->SetCurrentPosition(GUID_NULL, var);
		PropVariantClear(&var);

		videoInfo.isReset = false;
		videoInfo.PlayTime = 0;
		mVideoTimeStamp = 0;
	}

	HRESULT hr = S_OK;
	IMFSample* pSample = nullptr;

	// 비디오 샘플 읽기
	if (videoInfo.PlayTime * 10000000 >= mVideoTimeStamp)
	{
		DWORD dwFlags = 0;
		hr = pSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &mVideoIndex, &dwFlags, &mVideoTimeStamp, &pSample);
		if (mVideoTimeStamp < videoInfo.PlayTime * 10000000)
		{
			hr = pSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &mVideoIndex, &dwFlags, &mVideoTimeStamp, &pSample);
		}
		if (SUCCEEDED(hr) && pSample) {
			IMFMediaBuffer* pBuffer = nullptr;
			hr = pSample->ConvertToContiguousBuffer(&pBuffer);

			if (SUCCEEDED(hr)) {
				BYTE* pData = nullptr;
				DWORD bufferLength = 0;

				// 버퍼 잠금
				hr = pBuffer->Lock(&pData, nullptr, &bufferLength);
				if (SUCCEEDED(hr)) {
					// 비디오 데이터를 Direct2D 비트맵으로 변환
					if (mVideoBitmap != nullptr) {
						mVideoBitmap->Release();
						mVideoBitmap = nullptr;
					}

					D2D1_BITMAP_PROPERTIES properties = D2D1::BitmapProperties();
					properties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);

					hr = mRenderTarget->CreateBitmap(D2D1::SizeU(mVideoWidth, mVideoHeight), pData, mVideoWidth * 4, properties, &mVideoBitmap);

					// 버퍼 잠금 해제
					pBuffer->Unlock();
				}

				pBuffer->Release();
			}

			pSample->Release();
		}
	}

	mVideoObject->SetVideoInfo(videoInfo);
}

fq::graphics::IVideoObject* fq::graphics::UIManager::CreateVideoObject(const VideoInfo& videoInfo)
{
	if (pSourceReader != NULL)
	{
		pSourceReader->Release();
	}

	VideoObject* newVideoObject = new VideoObject;
	newVideoObject->SetVideoInfo(videoInfo);
	LoadVideo(stringToWstring(videoInfo.VideoPath));

	mVideoObject = newVideoObject;
	return newVideoObject;
}

void fq::graphics::UIManager::DeleteVideoObject(IVideoObject* videoObject)
{
	if (mVideoObject != nullptr)
	{
		delete mVideoObject;
		mVideoObject = nullptr;
	}
	if (mVideoBitmap != nullptr)
	{
		mVideoBitmap->Release();
		mVideoBitmap = nullptr;
	}
}

