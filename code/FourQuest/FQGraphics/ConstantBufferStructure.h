#pragma once

#include <map>
#include <memory>
#include "CommonHeader.h"
#include "Light.h"

namespace fq::graphics
{
	template <typename T>
	struct ConstantBuffer
	{
	public:
		static std::string GetName()
		{
			std::string name = typeid(T).name();
			size_t spacePos = name.find_first_of(' ');
			size_t colonPos = name.find_last_of(':');
			size_t classNamePos = std::max<size_t>(spacePos, colonPos);

			if (classNamePos != std::string::npos)
			{
				name = name.substr(classNamePos + 1, name.length());
			}

			return name;
		}
	};

	struct cbModelTransform : public ConstantBuffer<cbModelTransform>
	{

		DirectX::SimpleMath::Matrix WorldMat;
		DirectX::SimpleMath::Matrix WorldInvTransposeMat;
	};

	struct cbSceneTransform : public ConstantBuffer<cbSceneTransform>
	{
		DirectX::SimpleMath::Matrix ViewProjMat;
	};

	struct cbBoneTransform : public ConstantBuffer<cbBoneTransform>
	{
		enum { MAX_BOND_COUNT = 128 };
		DirectX::SimpleMath::Matrix FinalTransforms[128];
	};

	struct cbShadowTransform : public ConstantBuffer<cbShadowTransform>
	{
		enum { CASCADE_COUNT = 3 };
		enum { MAX_SHADOW_COUNT = 3 };

		DirectX::SimpleMath::Matrix ShadowViewProj[CASCADE_COUNT * MAX_SHADOW_COUNT];
		int ShadowCount;
		int unused[3];
	};

	struct cbShadowTransformCascaseEnd : public ConstantBuffer<cbShadowTransformCascaseEnd>
	{
		DirectX::SimpleMath::Matrix ShadowViewProj[cbShadowTransform::CASCADE_COUNT * cbShadowTransform::MAX_SHADOW_COUNT];
		DirectX::SimpleMath::Vector4 CascadeEnds[cbShadowTransform::MAX_SHADOW_COUNT];
		int ShadowCount;
		float unused[3];
	};

	struct cbAlpha : public ConstantBuffer<cbAlpha>
	{
		int bUseAlphaConstant;
		float Alpha;
		float unused[2];
	};

	struct ViewRotationProjectionMatrix
	{
		DirectX::SimpleMath::Matrix ViewProjMat;
	};

	struct cbModelTexture : public ConstantBuffer<cbModelTexture>
	{
		int bUseAlbedoMap;
		int bUseMetalnessMap;
		int bUseRoughnessMap;
		int bUseNormalMap;
		int bUseEmissiveMap;
		int bUseOpacityMap;
		int unused[2];
	};

	struct cbLight : public ConstantBuffer<cbLight>
	{
		DirectionalLight directionalLight[3];
		PointLight pointLight[10];
		SpotLight spotLight[5];

		unsigned int numOfDirectionalLight;
		unsigned int numOfPointLight;
		unsigned int numOfSpotLight;
		unsigned int isUseIBL;

		DirectX::SimpleMath::Vector3 eyePosition;
		float pad2;
	};

	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11Device;
	class Material;

	class ConstantBufferHelper
	{
	public:
		static void UpdateModelTransformCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<cbModelTransform>>& cbuffer,
			const DirectX::SimpleMath::Matrix& transform);
		static void UpdateModelTextureCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<cbModelTexture>>& cbuffer,
			const std::shared_ptr<Material>& material);
		static void UpdateBoneTransformCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<cbBoneTransform>>& cbuffer,
			const std::vector<DirectX::SimpleMath::Matrix>& finalTransforms);
	};
}