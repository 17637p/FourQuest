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

	drawAllImage(false);
	drawAllText();
	drawAllImage(true);

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

fq::graphics::ITextObject* fq::graphics::UIManager::CreateText(TextInfo textInfo)
{
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

void fq::graphics::UIManager::drawAllText()
{
	if (mRenderTarget)
	{
		mRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		ID2D1SolidColorBrush* tempBrush = nullptr;
		for (const auto& textObject : mTexts)
		{
			if (!textObject->GetTextInformation().IsRender)
			{
				continue;
			}

			TextInfo drawTextInformation = textObject->GetTextInformation();
			if (drawTextInformation.Text == "")
			{
				continue;
			}

			if (!drawTextInformation.IsRender)
			{
				continue;
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
			std::wstring fontPath = stringToWstring(drawTextInformation.FontPath) + std::to_wstring(drawTextInformation.FontSize);

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
}

bool IImageObjectCmpLayer(fq::graphics::IImageObject* a, fq::graphics::IImageObject* b)
{
	return a->GetLayer() > b->GetLayer();
}

void fq::graphics::UIManager::drawAllImage(bool isOnText)
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
		if (!image->GetIsRender())
		{
			continue;
		}

		if (image->GetIsOnText() != isOnText)
		{
			continue;
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

				//float radiusX = (image->GetWidth() / 2);// *image->GetScaleX();
				//float radiusY = (image->GetHeight() / 2);// *image->GetScaleY();

				float startX = image->GetStartX() - radiusX;
				float startY = image->GetStartY() - radiusY;

				//float maxLength = (radiusX > radiusY) ? radiusX : radiusY;
				//float halfDiagonal = (maxLength) * std::sqrt(2);

				// 경로 기하학을 생성하여 반원 모양을 정의합니다.
				ID2D1PathGeometry* pPathGeometry = nullptr;

				mDirect2DFactory->CreatePathGeometry(&pPathGeometry);
				ID2D1GeometrySink* pSink = nullptr;
				pPathGeometry->Open(&pSink);

				//float halfDiagonal = radiusX;
				//
				//startX = startX + radiusX - halfDiagonal;
				//startY = startY + radiusY - halfDiagonal;
				//
				//radiusX = halfDiagonal;
				//radiusY = halfDiagonal;

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

	newImageObject->SetIsOnText(uiInfo.isOnText);

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
	mTexts.erase(std::remove(mTexts.begin(), mTexts.end(), textObject));
	delete textObject;
}

std::wstring fq::graphics::UIManager::stringToWstring(std::string str)
{
	boost::locale::generator gen;
	std::locale loc = gen("en_US.UTF-8");
	std::locale::global(loc);

	// 문자열 변환
	std::string narrow_str = str;
	std::wstring wide_str = boost::locale::conv::to_utf<wchar_t>(narrow_str, "UTF-8");

	return wide_str;
}

