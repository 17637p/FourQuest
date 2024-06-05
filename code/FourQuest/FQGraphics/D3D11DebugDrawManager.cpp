#include "D3D11DebugDrawManager.h"
#include "D3D11CameraManager.h"
#include "D3D11Device.h"

namespace fq::graphics
{
	void D3D11DebugDrawManager::Initialize(const std::shared_ptr<D3D11Device>& device)
	{
		using namespace DirectX;

		mStates = std::make_unique<DirectX::DX11::CommonStates>(device->GetDevice().Get());
		mBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::DX11::VertexPositionColor>>(device->GetDeviceContext().Get());
		mBatchEffect = std::make_unique<DirectX::DX11::BasicEffect>(device->GetDevice().Get());
		mBatchEffect->SetVertexColorEnabled(true);

		void const* shaderByteCode;
		size_t byteCodeLength;
		mBatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		HRESULT hr = device->GetDevice()->CreateInputLayout(DirectX::DX11::VertexPositionColor::InputElements,
			DirectX::DX11::VertexPositionColor::InputElementCount,
			shaderByteCode,
			byteCodeLength,
			&mBatchInputLayout);
		assert(SUCCEEDED(hr));
	}
	void D3D11DebugDrawManager::Finalize()
	{
		mStates = nullptr;
		mBatchEffect = nullptr;
		mBatch = nullptr;
		mBatchInputLayout = nullptr;
	}

	void D3D11DebugDrawManager::Excute(const std::shared_ptr<D3D11Device>& device,
		const std::shared_ptr<D3D11CameraManager>& cameraManager)
	{
		device->GetDeviceContext()->OMSetBlendState(mStates->Opaque(), nullptr, 0xFFFFFFFF);
		device->GetDeviceContext()->OMSetDepthStencilState(mStates->DepthDefault(), 0);
		device->GetDeviceContext()->RSSetState(mStates->CullNone());

		mBatchEffect->Apply(device->GetDeviceContext().Get());

		mBatchEffect->SetView(cameraManager->GetViewMatrix(ECameraType::Player));
		mBatchEffect->SetProjection(cameraManager->GetProjectionMatrix(ECameraType::Player));

		device->GetDeviceContext()->IASetInputLayout(mBatchInputLayout.Get());

		mBatch->Begin();

		// draw
		{
			while (!mSphereInfos.empty()) { Draw(device, mSphereInfos.front()); mSphereInfos.pop(); }
			while (!mAABBInfos.empty()) { Draw(device, mAABBInfos.front()); mAABBInfos.pop(); }
			while (!mOBBInfos.empty()) { Draw(device, mOBBInfos.front()); mOBBInfos.pop(); }
			while (!mFrustumInfos.empty()) { Draw(device, mFrustumInfos.front()); mFrustumInfos.pop(); }
			while (!mGridInfos.empty()) { DrawGrid(device, mGridInfos.front()); mGridInfos.pop(); }
			while (!mRingInfos.empty()) { DrawRing(device, mRingInfos.front()); mRingInfos.pop(); }
			while (!mRayInfos.empty()) { DrawRay(device, mRayInfos.front()); mRayInfos.pop(); }
			while (!mPolygonInfos.empty()) { DrawLineStrip(device, mPolygonInfos.front()); mPolygonInfos.pop(); }
			while (!mHemisphereInfos.empty()) { Draw(device, mHemisphereInfos.front()); mHemisphereInfos.pop(); }
			while (!mConeInfos.empty()) { Draw(device, mConeInfos.front()); mConeInfos.pop(); }
			while (!mDountInfos.empty()) { Draw(device, mDountInfos.front()); mDountInfos.pop(); }
			while (!mSphereInfoExInfos.empty()) { Draw(device, mSphereInfoExInfos.front()); mSphereInfoExInfos.pop(); }
		}

		mBatch->End();

		device->GetDeviceContext()->OMSetBlendState(NULL, nullptr, 0xFFFFFFFF);
		device->GetDeviceContext()->OMSetDepthStencilState(NULL, 0);
		device->GetDeviceContext()->RSSetState(NULL);
	}

	void D3D11DebugDrawManager::Submit(const debug::SphereInfo& info)
	{
		mSphereInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::AABBInfo& info)
	{
		mAABBInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::OBBInfo& info)
	{
		mOBBInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::FrustumInfo& info)
	{
		mFrustumInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::GridInfo& info)
	{
		mGridInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::RingInfo& info)
	{
		mRingInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::RayInfo& info)
	{
		mRayInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::PolygonInfo& info)
	{
		mPolygonInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::HemisphereInfo& info)
	{
		mHemisphereInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::ConeInfo& info)
	{
		mConeInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::DountInfo& info)
	{
		mDountInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::SphereInfoEx& info)
	{
		mSphereInfoExInfos.push(info);
	}
	void D3D11DebugDrawManager::Submit(const debug::RingInfoEx& info)
	{
		mRingInfoExInfos.push(info);
	}

	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::SphereInfo& info)
	{

	}
	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::AABBInfo& info)
	{
		DirectX::SimpleMath::Matrix matWorld =
			DirectX::SimpleMath::Matrix::CreateScale(info.AABB.Extents) *
			DirectX::SimpleMath::Matrix::CreateTranslation(info.AABB.Center);

		drawCube(device, matWorld, info.Color);
	}
	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::OBBInfo& info)
	{
		DirectX::SimpleMath::Matrix matWorld =
			DirectX::SimpleMath::Matrix::CreateScale(info.OBB.Extents) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(info.OBB.Orientation) *
			DirectX::SimpleMath::Matrix::CreateTranslation(info.OBB.Center);

		drawCube(device, matWorld, info.Color);
	}
	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::FrustumInfo& info)
	{
		using namespace DirectX;

		DirectX::SimpleMath::Vector3 corners[DirectX::BoundingFrustum::CORNER_COUNT];
		info.Frustum.GetCorners(corners);

		VertexPositionColor verts[24] = {};
		verts[0].position = corners[0];
		verts[1].position = corners[1];
		verts[2].position = corners[1];
		verts[3].position = corners[2];
		verts[4].position = corners[2];
		verts[5].position = corners[3];
		verts[6].position = corners[3];
		verts[7].position = corners[0];

		verts[8].position = corners[0];
		verts[9].position = corners[4];
		verts[10].position = corners[1];
		verts[11].position = corners[5];
		verts[12].position = corners[2];
		verts[13].position = corners[6];
		verts[14].position = corners[3];
		verts[15].position = corners[7];

		verts[16].position = corners[4];
		verts[17].position = corners[5];
		verts[18].position = corners[5];
		verts[19].position = corners[6];
		verts[20].position = corners[6];
		verts[21].position = corners[7];
		verts[22].position = corners[7];
		verts[23].position = corners[4];

		for (size_t j = 0; j < std::size(verts); ++j)
		{
			XMStoreFloat4(&verts[j].color, info.Color);
		}

		mBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, verts, static_cast<UINT>(std::size(verts)));
	}
	void D3D11DebugDrawManager::DrawGrid(const std::shared_ptr<D3D11Device>& device, const debug::GridInfo& info)
	{
		using namespace DirectX;
		using namespace DirectX::SimpleMath;

		size_t xDivision = std::max<size_t>(1, info.XDivision);
		size_t yDivision = std::max<size_t>(1, info.YDivision);

		for (size_t i = 0; i <= xDivision; ++i)
		{
			float percent = float(i) / float(xDivision);
			percent = (percent * 2.f) - 1.f;
			Vector3 scale = info.XAxis * percent * info.GridSize * 0.5f;
			scale += info.Origin;

			VertexPositionColor v1(scale - info.YAxis * info.GridSize * 0.5f, info.Color);
			VertexPositionColor v2(scale + info.YAxis * info.GridSize * 0.5f, info.Color);

			mBatch->DrawLine(v1, v2);
		}

		for (size_t i = 0; i <= yDivision; i++)
		{
			FLOAT percent = float(i) / float(yDivision);
			percent = (percent * 2.f) - 1.f;
			Vector3 scale = info.YAxis * percent * info.GridSize * 0.5f;
			scale += info.Origin;

			VertexPositionColor v1(scale - info.XAxis * info.GridSize * 0.5f, info.Color);
			VertexPositionColor v2(scale + info.XAxis * info.GridSize * 0.5f, info.Color);

			mBatch->DrawLine(v1, v2);
		}
	}
	void D3D11DebugDrawManager::DrawRing(const std::shared_ptr<D3D11Device>& device, const debug::RingInfo& info)
	{
		using namespace DirectX;

		mBatchEffect->Apply(device->GetDeviceContext().Get());
		device->GetDeviceContext()->IASetInputLayout(mBatchInputLayout.Get());
		static const size_t c_ringSegments = 16;

		VertexPositionColor verts[c_ringSegments + 2];

		FLOAT fAngleDelta = info.Radian / float(c_ringSegments);
		// Instead of calling cos/sin for each segment we calculate
		// the sign of the angle delta and then incrementally calculate sin
		// and cosine from then on.
		XMVECTOR cosDelta = XMVectorReplicate(cosf(fAngleDelta));
		XMVECTOR sinDelta = XMVectorReplicate(sinf(fAngleDelta));
		XMVECTOR incrementalSin = XMVectorZero();
		static const XMVECTORF32 s_initialCos =
		{
			1.f, 1.f, 1.f, 1.f
		};
		XMVECTOR incrementalCos = s_initialCos.v;
		for (size_t i = 0; i < c_ringSegments + 1; i++)
		{
			XMVECTOR pos = XMVectorMultiplyAdd(info.MajorAxis, incrementalCos, info.Origin);
			pos = XMVectorMultiplyAdd(info.MinorAxis, incrementalSin, pos);
			XMStoreFloat3(&verts[i].position, pos);
			XMStoreFloat4(&verts[i].color, info.Color);
			// Standard formula to rotate a vector.
			XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
			XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
			incrementalCos = newCos;
			incrementalSin = newSin;
		}
		verts[c_ringSegments + 1] = verts[0];

		mBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
	}
	void D3D11DebugDrawManager::DrawRay(const std::shared_ptr<D3D11Device>& device, const debug::RayInfo& info)
	{
		using namespace DirectX;
		using namespace SimpleMath;

		VertexPositionColor verts[3];
		XMStoreFloat3(&verts[0].position, info.Origin);

		Vector3 normDirection;
		info.Direction.Normalize(normDirection);
		Vector3 rayDirection = (info.Normalize) ? normDirection : info.Direction;

		Vector3 perpVector = normDirection.Cross(Vector3::UnitY);

		if (XMVector3Equal(XMVector3LengthSq(perpVector), g_XMZero))
		{
			perpVector = XMVector3Cross(normDirection, g_XMIdentityR2);
		}
		perpVector = XMVector3Normalize(perpVector);

		XMStoreFloat3(&verts[1].position, XMVectorAdd(rayDirection, info.Origin));
		perpVector = XMVectorScale(perpVector, 0.0625f);
		normDirection = XMVectorScale(normDirection, -0.25f);
		rayDirection = XMVectorAdd(perpVector, rayDirection);
		rayDirection = XMVectorAdd(normDirection, rayDirection);
		XMStoreFloat3(&verts[2].position, XMVectorAdd(rayDirection, info.Origin));

		XMStoreFloat4(&verts[0].color, info.Color);
		XMStoreFloat4(&verts[1].color, info.Color);
		XMStoreFloat4(&verts[2].color, info.Color);

		mBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);
	}

	void D3D11DebugDrawManager::DrawLineStrip(const std::shared_ptr<D3D11Device>& device, const debug::PolygonInfo& info)
	{
		using namespace DirectX;
		std::vector<VertexPositionColor> vertices(info.Points.size() + 1);

		for (size_t i = 0; i < info.Points.size(); ++i)
		{
			XMStoreFloat3(&vertices[i].position, info.Points[i]);
			XMStoreFloat4(&vertices[i].color, info.Color);
		}
		XMStoreFloat3(&vertices.back().position, info.Points[0]);
		XMStoreFloat4(&vertices.back().color, info.Color);

		mBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, vertices.data(), vertices.size());
	}

	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::HemisphereInfo& info)
	{
		mBatchEffect->Apply(device->GetDeviceContext().Get());
		device->GetDeviceContext()->IASetInputLayout(mBatchInputLayout.Get());

		debug::RingInfo ringInfo;
		ringInfo.Color = info.Color;
		ringInfo.Origin = info.Origin;
		ringInfo.Radian = DirectX::XM_PIDIV2;

		if (info.Radian < DirectX::XM_PI * 0.5f)
		{
			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.XAxis;
			DrawRing(device, ringInfo);
		}
		else if (info.Radian < DirectX::XM_PI * 1.f)
		{
			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.XAxis;
			DrawRing(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.ZAxis;
			DrawRing(device, ringInfo);
		}
		else if (info.Radian < DirectX::XM_PI * 1.5f)
		{
			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.XAxis;
			DrawRing(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.ZAxis;
			DrawRing(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = -info.XAxis;
			DrawRing(device, ringInfo);
		}
		else
		{
			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.XAxis;
			DrawRing(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.ZAxis;
			DrawRing(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = -info.XAxis;
			DrawRing(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = -info.ZAxis;
			DrawRing(device, ringInfo);
		}

		using namespace DirectX::SimpleMath;

		// 비례 변환 아니면 오차 생기므로 끝점으로 축 구해서 렌더링 하는 식으로 수정해야 됨
		DirectX::SimpleMath::Vector3 axis = Vector3::Transform(info.XAxis, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(info.YAxis, -info.Radian));
		ringInfo.MajorAxis = info.YAxis;
		ringInfo.MinorAxis = axis;
		DrawRing(device, ringInfo);

		ringInfo.MajorAxis = info.XAxis;
		ringInfo.MinorAxis = info.ZAxis;
		ringInfo.Radian = info.Radian;
		DrawRing(device, ringInfo);
	}
	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::ConeInfo& info)
	{
		using namespace DirectX::SimpleMath;

		DirectX::SimpleMath::Vector3 xAxis = DirectX::SimpleMath::Vector3::UnitX * info.Radius;
		DirectX::SimpleMath::Vector3 yAxis = DirectX::SimpleMath::Vector3::UnitY * info.Radius;
		DirectX::SimpleMath::Vector3 zAxis = DirectX::SimpleMath::Vector3::UnitZ * info.Radius;

		debug::RingInfo ringInfo;
		ringInfo.Radian = info.ArcInRadian;
		ringInfo.Color = info.Color;
		ringInfo.Origin = { info.Transform._41, info.Transform._42, info.Transform._43 };
		ringInfo.MajorAxis = xAxis;
		ringInfo.MinorAxis = zAxis;
		DrawRing(device, ringInfo);

		debug::RingInfo extendRingInfo = ringInfo;
		extendRingInfo.Origin.y += info.Height;
		float extend = tan(info.AngleInRadian) * info.Height;
		Vector3 extendXAxis = xAxis + Vector3{ extend, 0, 0 };
		Vector3 extendZAxis = zAxis + Vector3{ 0, 0, extend };
		extendRingInfo.MajorAxis = extendXAxis;
		extendRingInfo.MinorAxis = extendZAxis;
		DrawRing(device, extendRingInfo);

		debug::PolygonInfo polygonInfo;
		polygonInfo.Points.resize(2);

		polygonInfo.Points[0] = ringInfo.Origin + xAxis;
		polygonInfo.Points[1] = extendRingInfo.Origin + extendXAxis;
		DrawLineStrip(device, polygonInfo);

		polygonInfo.Points[0] = ringInfo.Origin - xAxis;
		polygonInfo.Points[1] = extendRingInfo.Origin - extendXAxis;
		DrawLineStrip(device, polygonInfo);

		polygonInfo.Points[0] = ringInfo.Origin + zAxis;
		polygonInfo.Points[1] = extendRingInfo.Origin + extendZAxis;
		DrawLineStrip(device, polygonInfo);

		polygonInfo.Points[0] = ringInfo.Origin + zAxis;
		polygonInfo.Points[1] = extendRingInfo.Origin - extendZAxis;
		DrawLineStrip(device, polygonInfo);
	}
	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::DountInfo& info)
	{
		using namespace DirectX::SimpleMath;

		// 링 4개, 원 최대 4개
		DirectX::SimpleMath::Vector3 xAxis = DirectX::SimpleMath::Vector3::UnitX * info.Radius * info.Transform._11;
		DirectX::SimpleMath::Vector3 yAxis = DirectX::SimpleMath::Vector3::UnitY * info.Radius * info.Transform._22;
		DirectX::SimpleMath::Vector3 zAxis = DirectX::SimpleMath::Vector3::UnitZ * info.Radius * info.Transform._33;

		DirectX::SimpleMath::Vector3 xAxisPlusDountRadius = DirectX::SimpleMath::Vector3::UnitX * (info.Radius + info.DountRadius) * info.Transform._11;
		DirectX::SimpleMath::Vector3 zAxisPlusDountRadius = DirectX::SimpleMath::Vector3::UnitZ * (info.Radius + info.DountRadius) * info.Transform._33;

		DirectX::SimpleMath::Vector3 xAxisMinusDountRadius = DirectX::SimpleMath::Vector3::UnitX * (info.Radius - info.DountRadius) * info.Transform._11;
		DirectX::SimpleMath::Vector3 zAxisMinusDountRadius = DirectX::SimpleMath::Vector3::UnitZ * (info.Radius - info.DountRadius) * info.Transform._33;

		debug::RingInfo ringInfo;
		ringInfo.Color = info.Color;
		ringInfo.Origin = { info.Transform._41, info.Transform._42, info.Transform._43 };

		ringInfo.MajorAxis = xAxisPlusDountRadius; ringInfo.MinorAxis = zAxisPlusDountRadius; ringInfo.Radian = info.ArcInRadian;
		DrawRing(device, ringInfo);

		ringInfo.MajorAxis = xAxisMinusDountRadius; ringInfo.MinorAxis = zAxisMinusDountRadius; ringInfo.Radian = info.ArcInRadian;
		DrawRing(device, ringInfo);

		ringInfo.MajorAxis = xAxis; ringInfo.MinorAxis = zAxis; ringInfo.Radian = info.ArcInRadian;
		ringInfo.Origin.y += info.DountRadius;
		DrawRing(device, ringInfo);

		ringInfo.MajorAxis = xAxis; ringInfo.MinorAxis = zAxis; ringInfo.Radian = info.ArcInRadian;
		ringInfo.Origin.y -= info.DountRadius * 2.f;
		DrawRing(device, ringInfo);
	}

	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::SphereInfoEx& info)
	{
		mBatchEffect->Apply(device->GetDeviceContext().Get());
		device->GetDeviceContext()->IASetInputLayout(mBatchInputLayout.Get());

		debug::RingInfoEx ringInfo;
		ringInfo.Color = info.Color;
		ringInfo.Origin = info.Origin;
		ringInfo.ArcInRadian = DirectX::XM_PI;

		if (info.ArcInRadian < DirectX::XM_PI * 0.5f)
		{
			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.XAxis;
			Draw(device, ringInfo);
		}
		else if (info.ArcInRadian < DirectX::XM_PI * 1.f)
		{
			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.XAxis;
			Draw(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.ZAxis;
			Draw(device, ringInfo);
		}
		else if (info.ArcInRadian < DirectX::XM_PI * 1.5f)
		{
			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.XAxis;
			Draw(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.ZAxis;
			Draw(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = -info.XAxis;
			Draw(device, ringInfo);
		}
		else
		{
			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.XAxis;
			Draw(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = info.ZAxis;
			Draw(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = -info.XAxis;
			Draw(device, ringInfo);

			ringInfo.MajorAxis = info.YAxis;
			ringInfo.MinorAxis = -info.ZAxis;
			Draw(device, ringInfo);
		}

		using namespace DirectX::SimpleMath;

		// 비례 변환 아니면 오차 생기므로 끝점으로 축 구해서 렌더링 하는 식으로 수정해야 됨
		DirectX::SimpleMath::Vector3 axis = Vector3::Transform(info.XAxis, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(info.YAxis, -info.Radian));
		ringInfo.MajorAxis = info.YAxis;
		ringInfo.MinorAxis = axis;
		Draw(device, ringInfo);

		ringInfo.MajorAxis = info.XAxis;
		ringInfo.MinorAxis = info.ZAxis;
		ringInfo.ArcInRadian = info.ArcInRadian;
		Draw(device, ringInfo);
	}

	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::RingInfoEx& info)
	{

	}

	void D3D11DebugDrawManager::drawCube(const std::shared_ptr<D3D11Device>& device,
		const DirectX::SimpleMath::Matrix& worldTransform,
		const DirectX::SimpleMath::Color& color)
	{
		using namespace DirectX;

		static const XMVECTORF32 s_verts[8] =
		{
			{ { { -1.f, -1.f, -1.f, 0.f } } },
			{ { {  1.f, -1.f, -1.f, 0.f } } },
			{ { {  1.f, -1.f,  1.f, 0.f } } },
			{ { { -1.f, -1.f,  1.f, 0.f } } },
			{ { { -1.f,  1.f, -1.f, 0.f } } },
			{ { {  1.f,  1.f, -1.f, 0.f } } },
			{ { {  1.f,  1.f,  1.f, 0.f } } },
			{ { { -1.f,  1.f,  1.f, 0.f } } }
		};

		static const WORD s_indices[] =
		{
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			0, 4,
			1, 5,
			2, 6,
			3, 7
		};

		VertexPositionColor verts[8];
		for (size_t i = 0; i < 8; ++i)
		{
			XMVECTOR v = XMVector3Transform(s_verts[i], worldTransform);
			XMStoreFloat3(&verts[i].position, v);
			XMStoreFloat4(&verts[i].color, color);
		}

		mBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, static_cast<UINT>(std::size(s_indices)), verts, 8);
	}
}