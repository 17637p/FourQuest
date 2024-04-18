#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include <windows.h>

#include "ResourceBase.h"

#include "D3D11ResourceType.h"

namespace fq_graphics
{
	class D3D11Device;

	/// <summary>
	/// D3D11 Device 로 생성되는 리소스를 관리하는 클래스
	/// </summary>
	class D3D11ResourceManager : public std::enable_shared_from_this<D3D11ResourceManager>
	{
	public:
		D3D11ResourceManager(std::shared_ptr<D3D11Device> d3d11Device);

		template<typename T, typename EType>
		std::shared_ptr<T> Get(EType eType);

		template<typename T, typename EType, typename ...Params>
		std::shared_ptr<T> Create(EType eType, Params... params);

	private:
		std::unordered_map<std::string, std::weak_ptr<ResourceBase> > mResources;

		// 생성할 때 필요한 외부 팩토리나 생성에 관련된 클래스들
		std::shared_ptr<D3D11Device> mDevice;
	};

	template<typename T, typename EType>
	std::shared_ptr<T> D3D11ResourceManager::Get(EType eType)
	{
		std::string key = T::GenerateRID(eType);
		std::shared_ptr<T> resource = std::static_pointer_cast<T>(mResources[key].lock());

		if (!resource)
		{
			MessageBox(NULL, L"리소스 매니저에서 Get을 호출했지만 리소스가 존재하지 않습니다.", L"에러", MB_ICONERROR);
		}
		assert(resource);

		return resource;
	}

	template<typename T, typename EType, typename ...Params>
	std::shared_ptr<T> D3D11ResourceManager::Create(EType eType, Params... params)
	{
		std::string key = T::GenerateRID(eType);
		std::shared_ptr<ResourceBase> resource = mResources[key].lock();

		std::shared_ptr<T> resourceT;
		if (!resource)
		{
			resourceT = std::make_shared<T>(shared_from_this(), mDevice, eType, params...);
			mResources[key] = resourceT;
		}
		else
		{
			resourceT = std::static_pointer_cast<T>(mResources[key].lock());
		}

		return resourceT;
	}
}

