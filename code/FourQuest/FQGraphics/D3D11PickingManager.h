#pragma once

#include <set>
#include <unordered_map>
#include <memory>

#include <directxtk/SimpleMath.h>
#include <d3d11.h>
#include <wrl.h>

// temp
#include "d3d11Buffer.h"

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
	class ITerrainMeshObject;

	class D3D11ResourceManager;
	class D3D11CameraManager;
	class D3D11JobManager;

	class D3D11RenderTargetView;
	class D3D11DepthStencilView;
	class D3D11Device;

	class D3D11InputLayout;
	class D3D11VertexShader;
	class D3D11PixelShader;

	struct BoneTransform;

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
		void* GetPickedObject(
			const short x, const short y,
			const std::shared_ptr<D3D11Device>& device,
			const std::shared_ptr<D3D11CameraManager>& cameraManager,
			const std::shared_ptr<D3D11JobManager>& jobManager,
			const std::set<IStaticMeshObject*>& staticMeshObjects,
			const std::set<ISkinnedMeshObject*>& skinnedMeshObjects,
			const std::set<ITerrainMeshObject*>& terrainMeshObjects);
		void OnResize(const short width, const short height, const std::shared_ptr<D3D11Device>& device);

		void EndRender(const std::shared_ptr<D3D11Device>& device);

	private:
		// 1. 오브젝트 마다 해쉬 컬러 정하기
		// 일단은 그냥 오브젝트 전부를 받는데 컬링 하고 나면 이렇게 전부 받을 필요 없다.
		void MakeObjectsHashColor(const std::set<IStaticMeshObject*>& staticMeshObjects, 
			const std::set<ISkinnedMeshObject*>& skinnedMeshObjects,
			const std::set<ITerrainMeshObject*>& terrainMeshObjects);

		void DrawObject(const std::shared_ptr<D3D11Device>& device,
			const std::shared_ptr<D3D11CameraManager>& cameraManager,
			const std::shared_ptr<D3D11JobManager>& jobManager,
			const std::set<IStaticMeshObject*>& staticMeshObjects, 
			const std::set<ISkinnedMeshObject*>& skinnedMeshObjects,
			const std::set<ITerrainMeshObject*>& terrainMeshObjects);

		// 4. 그린 렌더 타겟에서 값 가져오기
		unsigned int GetHashColor(const std::shared_ptr<D3D11Device> device, const short x, const short y);
		unsigned int MakeRGBAUnsignedInt(DirectX::SimpleMath::Color color);
		void NextColor();

	private:
		// bool 변수로 pass 탈 지 말지 선택하면 될듯 
		// 2. 렌더 타겟 바인딩 
		// 3. 그리기 
		unsigned __int8 mR;
		unsigned __int8 mG;
		unsigned __int8 mB;
		        
		bool IsOnPicking;

		std::unordered_map<IStaticMeshObject*, DirectX::SimpleMath::Color> mStaticMeshObjects;
		std::unordered_map<ISkinnedMeshObject*, DirectX::SimpleMath::Color> mSkinnedMeshObjects;
		std::unordered_map<ITerrainMeshObject*, DirectX::SimpleMath::Color> mTerrainMeshObjects;

		//////////////////////////////////////////////////////////////////////////
		std::shared_ptr<D3D11RenderTargetView> mPickingDrawRTV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mCopyTexture;

		struct ModelTransform
		{
			DirectX::SimpleMath::Color color;
			DirectX::SimpleMath::Matrix world;
			DirectX::SimpleMath::Matrix ViewProj;
		};

		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11InputLayout> mSkinnedMeshLayout;
		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mConstantBuffer;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;

		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11VertexShader> mSkinnedMeshVS;
		std::shared_ptr<D3D11PixelShader> mMeshPS;

		// 마무리를 위한 렌더타겟과 뎁스스텐실뷰
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11DepthStencilView> mBackBufferDSV;
	};

	// 1. 오브젝트 마다 해쉬 컬러 정하기
	// 2. 렌더 타겟 바인딩 
	// 3. 그리기
	// 4. 그린 렌더 타겟에서 값 가져오기
	// 5. 해당 오브젝트 인스턴스 반환하기 
}