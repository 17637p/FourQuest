#include "D3D11TextManager.h"

#include <directxtk/SpriteBatch.h>
#include <directxtk/SpriteFont.h>
#include <directxtk/SimpleMath.h>
#include <tchar.h>

#include "D3D11Device.h"

fq::graphics::D3D11TextManager::D3D11TextManager(std::shared_ptr<D3D11Device> d3dDevice)
{
	mSpriteBatch = std::make_shared<DirectX::DX11::SpriteBatch>(d3dDevice->GetDeviceContext().Get());

	//temp: 일단 하드 코딩
	const wchar_t* fontFile = L"resource/internal/font/gulim9k.spritefont";

	mSpriteFont = std::make_shared<DirectX::DX11::SpriteFont>(d3dDevice->GetDevice().Get(), fontFile);
	mSpriteFont->SetLineSpacing(14.0f);
}

void fq::graphics::D3D11TextManager::DrawTextColor(const unsigned short x, const unsigned short y, const DirectX::SimpleMath::Color& color, const wchar_t* text, ...)
{
	wchar_t buffer[1024] = L"";
	va_list vl;
	va_start(vl, text);
	_vstprintf(buffer, 1024, text, vl);
	va_end(vl);

	mSpriteBatch->Begin();
	mSpriteFont->DrawString(mSpriteBatch.get(), buffer, DirectX::XMFLOAT2((float)x, (float)y), color);
	mSpriteBatch->End();
}

