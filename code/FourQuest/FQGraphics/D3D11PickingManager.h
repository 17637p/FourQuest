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
/// ���� ����Ʈ
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
	/// �� �������� �츮�� �����Ϳ����� Picking�� ����� �Ŵϱ� �ؽ�ó�� ��� �� �غ��� �� �� 
	/// ���߿� ���� ������ ������ �Ǹ� �ٲٸ� ���� �� ���� �� �˾Ҵµ�...
	/// </summary>
	class D3D11PickingManager
	{
	public:
		D3D11PickingManager();

		void Initialize(const std::shared_ptr<D3D11Device> device, 
			const std::shared_ptr<D3D11ResourceManager> resourceManager, 
			const unsigned short width, const unsigned short height);
		// 5. �ش� ������Ʈ �ν��Ͻ� ��ȯ�ϱ� 
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
		// 1. ������Ʈ ���� �ؽ� �÷� ���ϱ�
		// �ϴ��� �׳� ������Ʈ ���θ� �޴µ� �ø� �ϰ� ���� �̷��� ���� ���� �ʿ� ����.
		void MakeObjectsHashColor(const std::set<IStaticMeshObject*>& staticMeshObjects, 
			const std::set<ISkinnedMeshObject*>& skinnedMeshObjects,
			const std::set<ITerrainMeshObject*>& terrainMeshObjects);

		void DrawObject(const std::shared_ptr<D3D11Device>& device,
			const std::shared_ptr<D3D11CameraManager>& cameraManager,
			const std::shared_ptr<D3D11JobManager>& jobManager,
			const std::set<IStaticMeshObject*>& staticMeshObjects, 
			const std::set<ISkinnedMeshObject*>& skinnedMeshObjects,
			const std::set<ITerrainMeshObject*>& terrainMeshObjects);

		// 4. �׸� ���� Ÿ�ٿ��� �� ��������
		unsigned int GetHashColor(const std::shared_ptr<D3D11Device> device, const short x, const short y);
		unsigned int MakeRGBAUnsignedInt(DirectX::SimpleMath::Color color);
		void NextColor();

	private:
		// bool ������ pass Ż �� ���� �����ϸ� �ɵ� 
		// 2. ���� Ÿ�� ���ε� 
		// 3. �׸��� 
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

		// �������� ���� ����Ÿ�ٰ� �������ٽǺ�
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11DepthStencilView> mBackBufferDSV;
	};

	// 1. ������Ʈ ���� �ؽ� �÷� ���ϱ�
	// 2. ���� Ÿ�� ���ε� 
	// 3. �׸���
	// 4. �׸� ���� Ÿ�ٿ��� �� ��������
	// 5. �ش� ������Ʈ �ν��Ͻ� ��ȯ�ϱ� 
}