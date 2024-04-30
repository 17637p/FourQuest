#pragma once

#include <set>
#include <unordered_map>
#include <memory>

#include <directxtk/SimpleMath.h>
#include <d3d11.h>
#include <wrl.h>

/// <summary>
/// 참고 사이트
/// https://jaykop.github.io/post/graphics/3d-picking/
/// https://kyuhwang.tistory.com/45
/// 
/// </summary>

struct ID3D11Texture2D;

namespace fq::graphics
{
	class IStaticMeshObject;
	class ISkinnedMeshObject;

	class D3D11ResourceManager;
	class D3D11RenderTargetView;
	class D3D11Device;

	/// <summary>
	/// 내 생각에는 우리는 에디터에서만 Picking을 사용할 거니까 텍스처에 찍는 거 해봐도 될 듯 
	/// 나중에 많이 느려서 문제가 되면 바꾸면 되지 더 빠른 줄 알았는데...
	/// </summary>
	class D3D11PickingManager
	{
	public:
		D3D11PickingManager();

		void Initialize(const std::shared_ptr<D3D11Device> device, 
			const std::shared_ptr<D3D11ResourceManager> resourceManager, 
			const unsigned short width, const unsigned short height);
		// 5. 해당 오브젝트 인스턴스 반환하기 
		void* GetPickedObject(const std::shared_ptr<D3D11Device> device);

	private:
		// 1. 오브젝트 마다 해쉬 컬러 정하기
		// 일단은 그냥 오브젝트 전부를 받는데 컬링 하고 나면 이렇게 전부 받을 필요 없다.
		void MakeObjectsHashColor(const std::set<IStaticMeshObject*>& staticMeshObjects, 
			const std::set<ISkinnedMeshObject*>& skinnedMeshObjects);

		// 4. 그린 렌더 타겟에서 값 가져오기
		unsigned int GetHashColor(const std::shared_ptr<D3D11Device> device);
		unsigned int MakeRGBAUnsignedInt();
		void NextColor();

	private:
		// bool 변수로 pass 탈 지 말지 선택하면 될듯 
		// 2. 렌더 타겟 바인딩 
		// 3. 그리기 
		unsigned __int8 mR;
		unsigned __int8 mG;
		unsigned __int8 mB;
		        
		bool IsOnPicking;

		// color 는 hash 를 만들 수 없어서 unordered_map이 안된다. map 을 사용하면 속도가 느려지지만 에디터에서 조금 조금... 느린 건 괜찮지 않을까
		std::unordered_map<unsigned int, IStaticMeshObject*> mStaticMeshObjects;
		std::unordered_map<unsigned int, ISkinnedMeshObject*> mSkinnedMeshObjects;

		//////////////////////////////////////////////////////////////////////////
		std::shared_ptr<D3D11RenderTargetView> mPickingDrawRTV;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mCopyTexture;
	};

	// 1. 오브젝트 마다 해쉬 컬러 정하기
	// 2. 렌더 타겟 바인딩 
	// 3. 그리기
	// 4. 그린 렌더 타겟에서 값 가져오기
	// 5. 해당 오브젝트 인스턴스 반환하기 
}