#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include <directxtk/SimpleMath.h>

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;

	struct CubeProbe
	{
		unsigned int index;
		DirectX::SimpleMath::Vector3 position;
	};

	class D3D11LightProbeManager
	{
	public:
		D3D11LightProbeManager();
		~D3D11LightProbeManager();

		void Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager);

		unsigned short AddCubeProbe(const DirectX::SimpleMath::Vector3& position);
		void DeleteCubeProbe(unsigned short index);
		std::unordered_map<unsigned short, CubeProbe*> GetCubeProbes() const;

		std::wstring SaveCubeProbeTexture(const unsigned int index, const std::wstring& direction); //return path Name

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::unordered_map<unsigned short, CubeProbe*> mCubeProbes;
	};
}

