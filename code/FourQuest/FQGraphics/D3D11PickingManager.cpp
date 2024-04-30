#include "D3D11PickingManager.h"

#include "D3D11Device.h"
#include "D3D11ResourceManager.h"
#include "D3D11View.h"

#include "IFQRenderObject.h"

//윤서 코의 흔적
///Dear 유진Kim
/// 언리얼 수업 듣고 올라왔더니 너 왜 없냐
/// 이렇게 칼퇴해도 되는겁니까 예???
/// 근데 유진아 너 키보드 타건감 좋다
/// 키보드 정보 디코로 보내둬라.
/// 아닌가 조금 많이 무거운거 같기도 하고
/// 여튼 자리에 없는 우리 유진이
/// 코딩 화이팅 하고
/// 유진이가 좋아하는 그룹은 유진스
/// 깔깔깔 아이고 부장님
///  

fq::graphics::D3D11PickingManager::D3D11PickingManager()
	:mR(0),
	mG(0),
	mB(0)
{
}

void fq::graphics::D3D11PickingManager::Initialize(const std::shared_ptr<D3D11Device> device, const std::shared_ptr<D3D11ResourceManager> resourceManager, const unsigned short width, const unsigned short height)
{
	mPickingDrawRTV = resourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Picking, width, height);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDesc.MiscFlags = 0;
	
	device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &mCopyTexture);
}

void fq::graphics::D3D11PickingManager::MakeObjectsHashColor(const std::set<IStaticMeshObject*>& staticMeshObjects, const std::set<ISkinnedMeshObject*>& skinnedMeshObjects)
{
	for (const auto& meshObject : staticMeshObjects)
	{
		unsigned int objectColor = MakeRGBAUnsignedInt();
		mStaticMeshObjects[objectColor] = meshObject;
	}
	for (const auto& meshObject : skinnedMeshObjects)
	{
		unsigned int objectColor = MakeRGBAUnsignedInt();
		mSkinnedMeshObjects[objectColor] = meshObject;
	}
}

void fq::graphics::D3D11PickingManager::NextColor()
{
	if (mB = 255)
	{
		mB++;
		if (mG = 255)
		{
			mG++;
			if (mR = 255)
			{
				MessageBox(NULL, L"오브젝트의 개수가 1600만 개를 넘은 것 같습니다.", L"에러", MB_ICONERROR);
			}
			else
			{
				mR++;
			}
		}
		else
		{
			mG++;
		}
	}
	else
	{
		mB++;
	}
}

unsigned int fq::graphics::D3D11PickingManager::GetHashColor(const std::shared_ptr<D3D11Device> device)
{
	ID3D11DeviceContext* deviceContext = device->GetDeviceContext().Get();

	ID3D11Resource* pickingDrawResource = nullptr;
	mPickingDrawRTV->GetRTV()->GetResource(&pickingDrawResource);

	deviceContext->CopyResource(mCopyTexture.Get(), pickingDrawResource);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	deviceContext->Map(mCopyTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	if (mappedResource.pData == nullptr)
	{
		return 0;
	}

	unsigned int pickedObjectColor = ((UINT*)mappedResource.pData)[0];

	deviceContext->Unmap(mCopyTexture.Get(), 0);

	return pickedObjectColor;
}

unsigned int fq::graphics::D3D11PickingManager::MakeRGBAUnsignedInt()
{
	NextColor();

	unsigned int rgba = 0;

	rgba = 1;
	rgba += mB << 8;
	rgba += mG << 16;
	rgba += mR << 24;

	return rgba;
}

void* fq::graphics::D3D11PickingManager::GetPickedObject(const std::shared_ptr<D3D11Device> device)
{
	unsigned int pickedhashColor = GetHashColor(device);
	
	auto staticMeshIter = mStaticMeshObjects.find(pickedhashColor);
	if (staticMeshIter != mStaticMeshObjects.end())
	{
		return mStaticMeshObjects[pickedhashColor];
	}

	auto skinedMeshIter = mSkinnedMeshObjects.find(pickedhashColor);
	if (skinedMeshIter != mSkinnedMeshObjects.end())
	{
		return mSkinnedMeshObjects[pickedhashColor];
	}

	// picked Object X 
	return nullptr;

	// Bind해야함 
	// 안쪽에서 드로우를 따로 해야하나?
}

