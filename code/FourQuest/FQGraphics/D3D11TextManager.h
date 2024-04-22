#pragma once

#include <memory>

namespace DirectX
{
	inline namespace DX11
	{
		class SpriteBatch;
		class SpriteFont;
	}

	namespace SimpleMath { struct Color; }
}

namespace fq::graphics
{
	class D3D11Device;

	class D3D11TextManager
	{
	public:
		D3D11TextManager(std::shared_ptr<D3D11Device> d3dDevice);

		void DrawTextColor(const unsigned short x, const unsigned short y, 
			const DirectX::SimpleMath::Color& color, 
			const wchar_t* text, ...);

	private:
		std::shared_ptr<DirectX::DX11::SpriteBatch> mSpriteBatch;
		std::shared_ptr<DirectX::DX11::SpriteFont> mSpriteFont;
	};
}

