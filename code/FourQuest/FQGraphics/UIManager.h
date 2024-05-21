#pragma once

#include <d2d1.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <directxtk/SimpleMath.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

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

	class IImageObject;

	/// <summary>
	/// �ϴ� 3D������ UI�� �ʿ� ���� ������ Direct2D�� �����. 
	/// �ٸ� �� �ʿ��ϴٸ� �� �� ��������
	/// </summary>
	class UIManager
	{
	private:
		struct DrawTextInformation
		{
			std::wstring text;
			DirectX::SimpleMath::Rectangle drawRect;
			short fontSize;
			std::wstring fontPath;
			DirectX::SimpleMath::Color color;
		};

	public:
		UIManager();
		~UIManager();

		void Initialize(HWND hWnd, std::shared_ptr<D3D11Device> device, const short width, const short height);
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

		void DrawText(const std::wstring& text, const DirectX::SimpleMath::Rectangle& drawRect, unsigned short fontSize, const std::wstring& fontPath, const DirectX::SimpleMath::Color& color);

		// Image
		void AddImage(IImageObject* imageObject);
		void DeleteImage(IImageObject* imageObject);

	private:
		void loadImage(const std::wstring& path);
		HRESULT createRenderTarget(std::shared_ptr<D3D11Device> device, const short width, const short height);
		void initializeText();
		void initializeImage();

		void drawAllText();
		void drawAllImage();

		void RegisterFont(const std::wstring& path);
		void ReleaseAllImage();

	private:
		HWND mHWnd;

		ID2D1Factory* mDirect2DFactory;
		ID2D1RenderTarget* mRenderTarget;

		// �÷��� Ű�� �귯�ø� ����� Ű - ��� ������ ������ ����� ������ �������� ������� ���� �Է��� �� �ְ� �ص� ��
		std::unordered_map<DirectX::SimpleMath::Color, ID2D1SolidColorBrush*, ColorHash> mBrushes;

		// Font
		IDWriteFactory* mDWriteFactory;
		std::unordered_map<std::wstring, IDWriteTextFormat*> mFonts; // Ű: ��Ʈ�̸� + ������ 
		std::vector<std::wstring> mFontPath;

		std::wstring mDefaultFontPath;
		unsigned short mDefaultFontSize;
		DirectX::SimpleMath::Color mDefaultFontColor;

		std::vector<DrawTextInformation> mDrawTextInformations;

		// Image
		// ���۷��� ī������ �ǰ� �ؾ���
		std::unordered_map<std::wstring, ID2D1Bitmap*> mBitmaps;
		IWICImagingFactory* mWICFactory;

		std::vector<IImageObject*> mImages;
		bool mIsSorted;
	};
}

