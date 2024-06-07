#include "DebugRenderPass.h"

#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "ParticleObject.h"

namespace fq::graphics
{
	void DebugRenderPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11JobManager> jobManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr<D3D11ParticleManager> particleManager,
		unsigned short width,
		unsigned short height)
	{
		mDevice = device;
		mJobManager = jobManager;
		mDebugDrawManager = debugDrawManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;
		mParticleManager = particleManager;

		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDefaultDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		OnResize(width, height);
	}

	void DebugRenderPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mDebugDrawManager = nullptr;
		mCameraManager = nullptr;
		mResourceManager = nullptr;

		mBackBufferRTV = nullptr;
		mDefaultDSV = nullptr;
	}

	void DebugRenderPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void DebugRenderPass::Render()
	{
		using namespace DirectX::SimpleMath;

		mBackBufferRTV->Bind(mDevice, mDefaultDSV);

		for (IParticleObject* particleObjectInterface : mParticleManager->GetParticleObjects())
		{
			ParticleObject* particleObject = (ParticleObject*)particleObjectInterface;

			if (!particleObject->GetIsRenderDebug())
			{
				continue;
			}

			const ParticleInfo::Shape& shapeData = particleObject->GetInfo().ShapeData;
			DirectX::SimpleMath::Matrix shapeTransform = Matrix::CreateScale(shapeData.Scale) * Matrix::CreateFromYawPitchRoll(shapeData.Rotation) * Matrix::CreateTranslation(shapeData.Position);
			DirectX::SimpleMath::Matrix finalTransform = shapeTransform * particleObject->GetTransform();

			switch (shapeData.ShapeType)
			{
			case ParticleInfo::Shape::EShape::Sphere:
			{
				debug::SphereInfoEx sphereInfo;
				sphereInfo.Origin = Vector3{ finalTransform._41, finalTransform._42, finalTransform._43 };
				sphereInfo.XAxis = Vector3::TransformNormal(sphereInfo.XAxis * shapeData.Radius, finalTransform);
				sphereInfo.YAxis = Vector3::TransformNormal(sphereInfo.YAxis * shapeData.Radius, finalTransform);
				sphereInfo.ZAxis = Vector3::TransformNormal(sphereInfo.ZAxis * shapeData.Radius, finalTransform);
				sphereInfo.ArcInRadian = shapeData.ArcInDegree * 3.141592f / 180.f;
				mDebugDrawManager->Submit(sphereInfo);
				break;
			}
			case ParticleInfo::Shape::EShape::Hemisphere:
			{
				debug::HemisphereInfo hemisphereInfo;
				hemisphereInfo.Origin = Vector3{ finalTransform._41, finalTransform._42, finalTransform._43 };
				hemisphereInfo.XAxis = Vector3::TransformNormal(hemisphereInfo.XAxis * shapeData.Radius, finalTransform);
				hemisphereInfo.YAxis = Vector3::TransformNormal(hemisphereInfo.YAxis * shapeData.Radius, finalTransform);
				hemisphereInfo.ZAxis = Vector3::TransformNormal(hemisphereInfo.ZAxis * shapeData.Radius, finalTransform);
				hemisphereInfo.ArcInRadian = shapeData.ArcInDegree * 3.141592f / 180.f;
				mDebugDrawManager->Submit(hemisphereInfo);
				break;
			}
			case ParticleInfo::Shape::EShape::Cone:
			{
				debug::ConeInfo coneInfo;
				coneInfo.Origin = Vector3{ finalTransform._41, finalTransform._42, finalTransform._43 };
				coneInfo.XAxis = Vector3::TransformNormal(coneInfo.XAxis * shapeData.Radius, finalTransform);
				coneInfo.YAxis = Vector3::TransformNormal(coneInfo.YAxis * shapeData.Radius, finalTransform);
				coneInfo.ZAxis = Vector3::TransformNormal(coneInfo.ZAxis * shapeData.Radius, finalTransform);
				coneInfo.AngleInRadian = shapeData.AngleInDegree * 3.141592f / 180.f;
				coneInfo.ArcInRadian = shapeData.ArcInDegree * 3.141592f / 180.f;
				coneInfo.Height = particleObject->GetInfo().MainData.StartSpeed[0];
				mDebugDrawManager->Submit(coneInfo);
				break;
			}
			case ParticleInfo::Shape::EShape::Donut:
			{
				debug::DountInfo dountInfo;
				dountInfo.Origin = Vector3{ finalTransform._41, finalTransform._42, finalTransform._43 };
				dountInfo.XAxis = Vector3::TransformNormal(dountInfo.XAxis * shapeData.Radius, finalTransform);
				dountInfo.YAxis = Vector3::TransformNormal(dountInfo.YAxis * shapeData.Radius, finalTransform);
				dountInfo.ZAxis = Vector3::TransformNormal(dountInfo.ZAxis * shapeData.Radius, finalTransform);
				dountInfo.ArcInRadian = shapeData.ArcInDegree * 3.141592f / 180.f;
				dountInfo.DountRadius = shapeData.DountRadius;
				mDebugDrawManager->Submit(dountInfo);
				break;
			}
			case ParticleInfo::Shape::EShape::Box:
			{
				debug::OBBInfo obbInfo;
				obbInfo.OBB.Transform(obbInfo.OBB, shapeTransform * particleObject->GetTransform());
				mDebugDrawManager->Submit(obbInfo);
				break;
			}
			case ParticleInfo::Shape::EShape::Circle:
			{
				debug::RingInfoEx ringInfo;
				ringInfo.Origin = Vector3{ finalTransform._41, finalTransform._42, finalTransform._43 };
				ringInfo.MajorAxis = Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX * shapeData.Radius, finalTransform);
				ringInfo.MinorAxis = Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ * shapeData.Radius, finalTransform);
				ringInfo.ArcInRadian = shapeData.ArcInDegree * 3.141592 / 180.f;
				mDebugDrawManager->Submit(ringInfo);
				break;
			}
			case ParticleInfo::Shape::EShape::Rectangle:
			{
				debug::OBBInfo obbInfo;
				obbInfo.OBB.Extents = { 1, 0, 1 };
				obbInfo.OBB.Transform(obbInfo.OBB, shapeTransform * particleObject->GetTransform());
				mDebugDrawManager->Submit(obbInfo);
				break;
			}
			}
		}

		mDebugDrawManager->Excute(mDevice, mCameraManager);
	}
}