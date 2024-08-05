#pragma once

#include <d2d1.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <directxtk/SimpleMath.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include "../FQCommon/FQCommonGraphics.h"

struct ID2D1Factory;
struct ID2D1HwndRenderTarget;
struct IDWriteFactory;
struct IDWriteTextFormat;
struct ID2D1SolidColorBrush;
struct IWICImagingFactory;

struct ColorHash
{
	std::size_t operator()(const DirectX::SimpleMath::Color& color) const
	{
		return std::hash<float>()(color.R()) ^
			std::hash<float>()(color.G()) ^
			std::hash<float>()(color.B()) ^
			std::hash<float>()(color.A());
	}
};

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;

	class IImageObject;
	class ITextObject;
	/// <summary>
	/// 일단 3D스러운 UI가 필요 없어 보여서 Direct2D로 만든다. 
	/// 다른 게 필요하다면 그 때 만들어야지
	/// </summary>
	class UIManager
	{
	private:
		struct FQBitmap
		{
			unsigned short refCount;
			ID2D1Bitmap* bitmap;
		};

	public:
		UIManager();
		~UIManager();

		void Initialize(HWND hWnd, std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager, 
			const short width, const short height);
		void Render();
		void Finalize();

		void ReleaseRenderTarget();
		void OnResize(std::shared_ptr<D3D11Device> device, const short width, const short height);

		// Font
		void AddFont(const std::wstring& path);
		void DeleteFont(const std::wstring& path);

		void SetDefaultFont(const std::wstring& fontPath); // 그냥 내가 한글 윈도우에 있는 기본 하나 설정해서 미리 설정해두기 
		void SetDefaultFontColor(const DirectX::SimpleMath::Color& color);
		void SetDefaultFontSize(const unsigned short fontSize);

		ITextObject* CreateText(TextInfo textInfo);
		void DeleteText(fq::graphics::ITextObject* textObject);

		// Image
		IImageObject* CreateImageObject(const UIInfo& uiInfo);
		void AddImage(IImageObject* imageObject);
		void DeleteImage(IImageObject* imageObject);

	private:
		void loadBitmap(const std::wstring& path);
		HRESULT createRenderTarget(std::shared_ptr<D3D11Device> device, const short width, const short height);
		void initializeText();
		void initializeImage();

		void drawAllText();
		void drawAllImage();

		void registerFont(const std::wstring& path);

	private:
		HWND mHWnd;

		ID2D1Factory* mDirect2DFactory;
		ID2D1RenderTarget* mRenderTarget;

		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		// 컬러를 키로 브러시를 밸류로 키 - 밸류 쌍으로 없으면 만들고 있으면 꺼내쓰는 방식으로 색을 입력할 수 있게 해둘 것
		std::unordered_map<DirectX::SimpleMath::Color, ID2D1SolidColorBrush*, ColorHash> mBrushes;

		// Font
		IDWriteFactory* mDWriteFactory;
		std::unordered_map<std::wstring, IDWriteTextFormat*> mFonts; // 키: 폰트이름 + 사이즈 
		std::vector<std::wstring> mFontPath;

		std::wstring mDefaultFontPath;
		unsigned short mDefaultFontSize;
		DirectX::SimpleMath::Color mDefaultFontColor;

		// Image
		// 레퍼런스 카운팅이 되게 해야함
		std::unordered_map<std::wstring, FQBitmap*> mBitmaps;
		IWICImagingFactory* mWICFactory;

		std::vector<IImageObject*> mImages;
		std::vector<ITextObject*> mTexts;
		bool mIsSorted;
	};
}

