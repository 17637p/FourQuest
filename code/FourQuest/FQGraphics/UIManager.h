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
	class ISpriteAnimationObject;
	/// <summary>
	/// �ϴ� 3D������ UI�� �ʿ� ���� ������ Direct2D�� �����. 
	/// �ٸ� �� �ʿ��ϴٸ� �� �� ��������
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

		void SetDefaultFont(const std::wstring& fontPath); // �׳� ���� �ѱ� �����쿡 �ִ� �⺻ �ϳ� �����ؼ� �̸� �����صα� 
		void SetDefaultFontColor(const DirectX::SimpleMath::Color& color);
		void SetDefaultFontSize(const unsigned short fontSize);

		ITextObject* CreateText(TextInfo textInfo);
		void DeleteText(ITextObject* textObject);

		// Image
		IImageObject* CreateImageObject(const UIInfo& uiInfo);
		void AddImage(IImageObject* imageObject);
		void DeleteImage(IImageObject* imageObject);

		// Sprite Animation
		ISpriteAnimationObject* CreateSpriteAniamtion(SpriteInfo spriteInfo);
		void DeleteSpriteAniamtion(ISpriteAnimationObject* spriteAniamtionObject);

		void SetIsRenderObjects(bool isRenderObjects) { mIsRenderObjects = isRenderObjects; }

		/// UI FQR �۾� (���ҽ� ���� �۾�)
		// UI.fqr Load, Unload
		void SaveUIFQR(std::string path);
		void LoadUIFQR(std::string path);
		void UnLoadUIFQR();

		// fqr �κ��� bitmap ����
		void LoadBitmapFromFQR(std::string originalImagePath);

	private:
		void loadBitmap(const std::wstring& path);
		HRESULT createRenderTarget(std::shared_ptr<D3D11Device> device, const short width, const short height);
		void initializeText();
		void initializeImage();

		void draw();
		void drawText(ITextObject* textObject);
		void drawImage(IImageObject* imageObject);
		void drawSpriteAnimation(ISpriteAnimationObject* spriteAnimationObject);

		void registerFont(const std::wstring& path);

		std::wstring stringToWstring(std::string str);

	private:
		HWND mHWnd;

		// RenderObject ���� // false��� UIManger���� RenderTarget�� �ʱ�ȭ�ؾ���
		bool mIsRenderObjects;

		ID2D1Factory* mDirect2DFactory;
		ID2D1RenderTarget* mRenderTarget;

		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		// �÷��� Ű�� �귯�ø� ����� Ű - ��� ������ ������ ����� ������ �������� ������� ���� �Է��� �� �ְ� �ص� ��
		std::unordered_map<DirectX::SimpleMath::Color, ID2D1SolidColorBrush*, ColorHash> mBrushes;

		// Font
		IDWriteFactory* mDWriteFactory;
		std::unordered_map<std::wstring, IDWriteTextFormat*> mFonts; // Ű: ��Ʈ�̸� + ������ 
		std::vector<std::wstring> mFontPath;

		std::wstring mDefaultFontPath;
		unsigned short mDefaultFontSize;
		DirectX::SimpleMath::Color mDefaultFontColor;

		// Image
		// ���۷��� ī������ �ǰ� �ؾ���
		std::unordered_map<std::wstring, FQBitmap*> mBitmaps;
		IWICImagingFactory* mWICFactory;

		std::vector<IImageObject*> mImages;
		std::vector<ITextObject*> mTexts;
		std::vector<ISpriteAnimationObject*> mSpriteAnimations;

		bool mIsSortedSpriteAnimation;
		bool mIsSortedImage;
		bool mIsSortedText;
		bool mIsSortedAll;
	};
}

