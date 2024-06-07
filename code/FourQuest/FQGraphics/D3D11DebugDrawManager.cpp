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
			while (!mRingInfoExInfos.empty()) { Draw(device, mRingInfoExInfos.front()); mRingInfoExInfos.pop(); }
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

		FLOAT fAngleDelta = XM_2PI / float(c_ringSegments);
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

		debug::RingInfoEx ringInfo;
		ringInfo.Color = info.Color;
		ringInfo.Origin = info.Origin;
		ringInfo.ArcInRadian = DirectX::XM_PIDIV2;

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

		using namespace DirectX;
		using namespace DirectX::SimpleMath;
		XMVECTOR incrementalSin = XMVectorZero();
		XMVECTORF32 s_initialCos =
		{
			1.f, 1.f, 1.f, 1.f
		};
		XMVECTOR cosTheta = XMVectorReplicate(cosf(info.ArcInRadian));
		XMVECTOR sinTheta = XMVectorReplicate(sinf(info.ArcInRadian));
		XMVECTOR newCos = s_initialCos * cosTheta - incrementalSin * sinTheta;
		XMVECTOR newSin = s_initialCos * sinTheta + incrementalSin * cosTheta;
		XMVECTOR pos = XMVectorMultiplyAdd(info.XAxis, newCos, info.Origin);
		pos = XMVectorMultiplyAdd(info.ZAxis, newSin, pos);

		DirectX::SimpleMath::Vector3 axis = (Vector3)pos - info.Origin;
		ringInfo.MajorAxis = info.YAxis;
		ringInfo.MinorAxis = axis;
		Draw(device, ringInfo);

		ringInfo.MajorAxis = info.XAxis;
		ringInfo.MinorAxis = info.ZAxis;
		ringInfo.ArcInRadian = info.ArcInRadian;
		Draw(device, ringInfo);
	}
	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::ConeInfo& info)
	{
		using namespace DirectX::SimpleMath;

		debug::RingInfoEx ringInfo;
		ringInfo.Origin = info.Origin;
		ringInfo.MajorAxis = info.XAxis;
		ringInfo.MinorAxis = info.ZAxis;
		ringInfo.ArcInRadian = info.ArcInRadian;
		ringInfo.Color = info.Color;
		Draw(device, ringInfo);

		Vector3 normalizedXAxis;
		info.XAxis.Normalize(normalizedXAxis);
		Vector3 normalizedYAxis;
		info.YAxis.Normalize(normalizedYAxis);
		Vector3 normalizedZAxis;
		info.ZAxis.Normalize(normalizedZAxis);

		debug::RingInfoEx extendRingInfo = ringInfo;
		extendRingInfo.Origin += normalizedYAxis * info.Height;
		float extend = tan(info.AngleInRadian) * info.Height;
		extendRingInfo.MajorAxis = info.XAxis + normalizedXAxis * extend; //
		extendRingInfo.MinorAxis = info.ZAxis + normalizedZAxis * extend;
		Draw(device, extendRingInfo);

		debug::PolygonInfo polygonInfo;
		polygonInfo.Points.resize(2);

		if (info.ArcInRadian < DirectX::XM_PI * 0.5f)
		{
			polygonInfo.Points[0] = ringInfo.Origin + ringInfo.MajorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin + extendRingInfo.MajorAxis;
			DrawLineStrip(device, polygonInfo);
		}
		else if (info.ArcInRadian < DirectX::XM_PI * 1.f)
		{
			polygonInfo.Points[0] = ringInfo.Origin + ringInfo.MajorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin + extendRingInfo.MajorAxis;
			DrawLineStrip(device, polygonInfo);

			polygonInfo.Points[0] = ringInfo.Origin + ringInfo.MinorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin + extendRingInfo.MinorAxis;
			DrawLineStrip(device, polygonInfo);
		}
		else if (info.ArcInRadian < DirectX::XM_PI * 1.5f)
		{
			polygonInfo.Points[0] = ringInfo.Origin + ringInfo.MajorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin + extendRingInfo.MajorAxis;
			DrawLineStrip(device, polygonInfo);

			polygonInfo.Points[0] = ringInfo.Origin + ringInfo.MinorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin + extendRingInfo.MinorAxis;
			DrawLineStrip(device, polygonInfo);

			polygonInfo.Points[0] = ringInfo.Origin - ringInfo.MajorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin - extendRingInfo.MajorAxis;
			DrawLineStrip(device, polygonInfo);
		}
		else
		{
			polygonInfo.Points[0] = ringInfo.Origin + ringInfo.MajorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin + extendRingInfo.MajorAxis;
			DrawLineStrip(device, polygonInfo);

			polygonInfo.Points[0] = ringInfo.Origin + ringInfo.MinorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin + extendRingInfo.MinorAxis;
			DrawLineStrip(device, polygonInfo);

			polygonInfo.Points[0] = ringInfo.Origin - ringInfo.MajorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin - extendRingInfo.MajorAxis;
			DrawLineStrip(device, polygonInfo);

			polygonInfo.Points[0] = ringInfo.Origin - ringInfo.MinorAxis;
			polygonInfo.Points[1] = extendRingInfo.Origin - extendRingInfo.MinorAxis;
			DrawLineStrip(device, polygonInfo);
		}

		using namespace DirectX;
		using namespace DirectX::SimpleMath;
		XMVECTOR incrementalSin = XMVectorZero();
		XMVECTORF32 s_initialCos =
		{
			1.f, 1.f, 1.f, 1.f
		};
		XMVECTOR cosTheta = XMVectorReplicate(cosf(info.ArcInRadian));
		XMVECTOR sinTheta = XMVectorReplicate(sinf(info.ArcInRadian));
		XMVECTOR newCos = s_initialCos * cosTheta - incrementalSin * sinTheta;
		XMVECTOR newSin = s_initialCos * sinTheta + incrementalSin * cosTheta;
		XMVECTOR pos = XMVectorMultiplyAdd(ringInfo.MajorAxis, newCos, ringInfo.Origin);
		pos = XMVectorMultiplyAdd(ringInfo.MinorAxis, newSin, pos);
		XMVECTOR extendedPos = XMVectorMultiplyAdd(extendRingInfo.MajorAxis, newCos, extendRingInfo.Origin);
		extendedPos = XMVectorMultiplyAdd(extendRingInfo.MinorAxis, newSin, extendedPos);

		polygonInfo.Points[0] = pos;
		polygonInfo.Points[1] = extendedPos;
		DrawLineStrip(device, polygonInfo);
	}
	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::DountInfo& info)
	{
		using namespace DirectX::SimpleMath;

		Vector3 normalizedZAxis;
		info.ZAxis.Normalize(normalizedZAxis);
		Vector3 normalizedYAxis;
		info.YAxis.Normalize(normalizedYAxis);
		Vector3 normalizedXAxis;
		info.XAxis.Normalize(normalizedXAxis);

		debug::RingInfoEx ringInfo;
		ringInfo.ArcInRadian = info.ArcInRadian;
		ringInfo.MajorAxis = info.XAxis;
		ringInfo.MinorAxis = info.ZAxis;
		ringInfo.Color = info.Color;

		ringInfo.Origin = info.Origin + normalizedYAxis * info.DountRadius;
		Draw(device, ringInfo);

		ringInfo.Origin = info.Origin - normalizedYAxis * info.DountRadius;
		Draw(device, ringInfo);

		ringInfo.Origin = info.Origin;
		ringInfo.MajorAxis = info.XAxis + normalizedXAxis * info.DountRadius;
		ringInfo.MinorAxis = info.ZAxis + normalizedZAxis * info.DountRadius;
		Draw(device, ringInfo);

		ringInfo.Origin = info.Origin;
		ringInfo.MajorAxis = info.XAxis - normalizedXAxis * info.DountRadius;
		ringInfo.MinorAxis = info.ZAxis - normalizedZAxis * info.DountRadius;
		Draw(device, ringInfo);

		if (info.ArcInRadian < DirectX::XM_PI * 0.5f)
		{
			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedXAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = info.XAxis + info.Origin;
			Draw(device, ringInfo);
		}
		else if (info.ArcInRadian < DirectX::XM_PI * 1.f)
		{
			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedXAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = info.XAxis + info.Origin;
			Draw(device, ringInfo);

			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedZAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = info.ZAxis + info.Origin;
			Draw(device, ringInfo);
		}
		else if (info.ArcInRadian < DirectX::XM_PI * 1.5f)
		{
			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedXAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = info.XAxis + info.Origin;
			Draw(device, ringInfo);

			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedZAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = info.ZAxis + info.Origin;
			Draw(device, ringInfo);

			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedXAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = -info.XAxis + info.Origin;
			Draw(device, ringInfo);
		}
		else
		{
			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedXAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = info.XAxis + info.Origin;
			Draw(device, ringInfo);

			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedZAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = info.ZAxis + info.Origin;
			Draw(device, ringInfo);

			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedXAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = -info.XAxis + info.Origin;
			Draw(device, ringInfo);

			ringInfo.ArcInRadian = DirectX::XM_2PI;
			ringInfo.MajorAxis = normalizedZAxis * info.DountRadius;
			ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
			ringInfo.Origin = -info.ZAxis + info.Origin;
			Draw(device, ringInfo);
		}

		using namespace DirectX;
		using namespace DirectX::SimpleMath;
		XMVECTOR incrementalSin = XMVectorZero();
		XMVECTORF32 s_initialCos =
		{
			1.f, 1.f, 1.f, 1.f
		};
		XMVECTOR cosTheta = XMVectorReplicate(cosf(info.ArcInRadian));
		XMVECTOR sinTheta = XMVectorReplicate(sinf(info.ArcInRadian));
		XMVECTOR newCos = s_initialCos * cosTheta - incrementalSin * sinTheta;
		XMVECTOR newSin = s_initialCos * sinTheta + incrementalSin * cosTheta;
		XMVECTOR pos = XMVectorMultiplyAdd(info.XAxis, newCos, info.Origin);
		pos = XMVectorMultiplyAdd(info.ZAxis, newSin, pos);

		DirectX::SimpleMath::Vector3 axis = (Vector3)pos - info.Origin;
		DirectX::SimpleMath::Vector3 normalizedAxis;
		axis.Normalize(normalizedAxis);
		ringInfo.Origin = info.Origin + axis;
		ringInfo.MajorAxis = normalizedAxis * info.DountRadius;
		ringInfo.MinorAxis = normalizedYAxis * info.DountRadius;
		Draw(device, ringInfo);
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

		using namespace DirectX;
		using namespace DirectX::SimpleMath;
		XMVECTOR incrementalSin = XMVectorZero();
		XMVECTORF32 s_initialCos =
		{
			1.f, 1.f, 1.f, 1.f
		};
		XMVECTOR cosTheta = XMVectorReplicate(cosf(info.ArcInRadian));
		XMVECTOR sinTheta = XMVectorReplicate(sinf(info.ArcInRadian));
		XMVECTOR newCos = s_initialCos * cosTheta - incrementalSin * sinTheta;
		XMVECTOR newSin = s_initialCos * sinTheta + incrementalSin * cosTheta;
		XMVECTOR pos = XMVectorMultiplyAdd(info.XAxis, newCos, info.Origin);
		pos = XMVectorMultiplyAdd(info.ZAxis, newSin, pos);

		DirectX::SimpleMath::Vector3 axis = (Vector3)pos - info.Origin;
		ringInfo.MajorAxis = info.YAxis;
		ringInfo.MinorAxis = axis;
		ringInfo.Color = { 1, 0, 0,1 };
		Draw(device, ringInfo);

		ringInfo.MajorAxis = info.XAxis;
		ringInfo.MinorAxis = info.ZAxis;
		ringInfo.ArcInRadian = info.ArcInRadian;
		Draw(device, ringInfo);
	}

	void D3D11DebugDrawManager::Draw(const std::shared_ptr<D3D11Device>& device, const debug::RingInfoEx& info)
	{
		using namespace DirectX;

		mBatchEffect->Apply(device->GetDeviceContext().Get());
		device->GetDeviceContext()->IASetInputLayout(mBatchInputLayout.Get());
		static const size_t c_ringSegments = 16;

		VertexPositionColor verts[c_ringSegments + 2];

		FLOAT fAngleDelta = info.ArcInRadian / float(c_ringSegments);
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