#pragma once

#include <set>
#include <unordered_map>
#include <memory>

#include <directxtk/SimpleMath.h>
#include <d3d11.h>
#include <wrl.h>

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

	class D3D11ResourceManager;
	class D3D11RenderTargetView;
	class D3D11Device;

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
		void* GetPickedObject(const std::shared_ptr<D3D11Device> device);

	private:
		// 1. ������Ʈ ���� �ؽ� �÷� ���ϱ�
		// �ϴ��� �׳� ������Ʈ ���θ� �޴µ� �ø� �ϰ� ���� �̷��� ���� ���� �ʿ� ����.
		void MakeObjectsHashColor(const std::set<IStaticMeshObject*>& staticMeshObjects, 
			const std::set<ISkinnedMeshObject*>& skinnedMeshObjects);

		// 4. �׸� ���� Ÿ�ٿ��� �� ��������
		unsigned int GetHashColor(const std::shared_ptr<D3D11Device> device);
		unsigned int MakeRGBAUnsignedInt();
		void NextColor();

	private:
		// bool ������ pass Ż �� ���� �����ϸ� �ɵ� 
		// 2. ���� Ÿ�� ���ε� 
		// 3. �׸��� 
		unsigned __int8 mR;
		unsigned __int8 mG;
		unsigned __int8 mB;
		        
		bool IsOnPicking;

		// color �� hash �� ���� �� ��� unordered_map�� �ȵȴ�. map �� ����ϸ� �ӵ��� ���������� �����Ϳ��� ���� ����... ���� �� ������ ������
		std::unordered_map<unsigned int, IStaticMeshObject*> mStaticMeshObjects;
		std::unordered_map<unsigned int, ISkinnedMeshObject*> mSkinnedMeshObjects;

		//////////////////////////////////////////////////////////////////////////
		std::shared_ptr<D3D11RenderTargetView> mPickingDrawRTV;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mCopyTexture;
	};

	// 1. ������Ʈ ���� �ؽ� �÷� ���ϱ�
	// 2. ���� Ÿ�� ���ε� 
	// 3. �׸���
	// 4. �׸� ���� Ÿ�ٿ��� �� ��������
	// 5. �ش� ������Ʈ �ν��Ͻ� ��ȯ�ϱ� 
}