#pragma once

#include <memory>


namespace fq::graphics
{
	class D3D11Device;

	enum EPostEffectFlag
	{
		ToneMapping = 1,
	};

	class D3D11PostProcessingManager
	{
	public:
		D3D11PostProcessingManager() = default;
		~D3D11PostProcessingManager() = default;

		void Initialize(std::shared_ptr<D3D11Device> device);

		void Excute(std::shared_ptr<class D3D11RenderTargetView> rtv, std::shared_ptr<class D3D11ShaderResourceView> srv, EPostEffectFlag flag);
		void Excute(class D3D11RenderTargetView* rtv, class ID3D11ShaderResourceView* srv, EPostEffectFlag flag);

		void SetPostEffectFlag(EPostEffectFlag flag) { mPostEffectFlag = flag; }
		EPostEffectFlag GetPostEffectFlag() const { return mPostEffectFlag; }

	private:
		EPostEffectFlag mPostEffectFlag;
		std::shared_ptr<class D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<class D3D11IndexBuffer> mFullScreenIB;

		std::unique_ptr<class ShaderProgram> mToneMapping;

	};
}