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
	/// D3D11 Device �� �����Ǵ� ���ҽ��� �����ϴ� Ŭ����
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

		// ������ �� �ʿ��� �ܺ� ���丮�� ������ ���õ� Ŭ������
		std::shared_ptr<D3D11Device> mDevice;
	};

	template<typename T, typename EType>
	std::shared_ptr<T> D3D11ResourceManager::Get(EType eType)
	{
		std::string key = T::GenerateRID(eType);
		std::shared_ptr<T> resource = std::static_pointer_cast<T>(mResources[key].lock());

		if (!resource)
		{
			MessageBox(NULL, L"���ҽ� �Ŵ������� Get�� ȣ�������� ���ҽ��� �������� �ʽ��ϴ�.", L"����", MB_ICONERROR);
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

