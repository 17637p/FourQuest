#include "TrailObject.h"
#include "D3D11ResourceManager.h"

namespace fq::graphics
{
	TrailObject::TrailObject(const TrailInfo& trailInfo, std::shared_ptr<D3D11ResourceManager> resourceManager)
		: mTrailInfo(trailInfo)
		, mResourceManager(resourceManager)
		, mTexture(nullptr)
	{
		using namespace DirectX::SimpleMath;
		mLastPosition = Vector3::Transform(Vector3::Zero, mTrailInfo.Transform);
	}

	void TrailObject::Simulate(DirectX::SimpleMath::Vector3 cameraPos)
	{
		using namespace DirectX::SimpleMath;

		auto iter = mVertices.begin();
		for (; iter != mVertices.end(); ++iter)
		{
			if (iter->Age < mTrailInfo.FrameTime)
			{
				break;
			}

			iter->Age -= mTrailInfo.FrameTime;
		}
		mVertices.erase(iter, mVertices.end());

		DirectX::SimpleMath::Vector3 position = Vector3::Transform(Vector3::Zero, mTrailInfo.Transform);

		float distance = DirectX::SimpleMath::Vector3::Distance(position, mLastPosition);
		if (distance > mTrailInfo.MinVertexDistance)
		{
			size_t frameMaxDivideCount = std::min<size_t>(distance / mTrailInfo.MinVertexDistance, TrailInfo::MAX_VERTEX_SIZE / 60.f);

			for (size_t i = 0; i < frameMaxDivideCount; ++i)
			{
				float ratio = i / frameMaxDivideCount;

				Vertex topVertex;
				Vertex bottomVertex;
				topVertex.Age = mTrailInfo.Time;
				bottomVertex.Age = mTrailInfo.Time;

				switch (mTrailInfo.AlignmentType)
				{
				case TrailInfo::EAlignment::View:
				{
					Vector3 dir = position - mLastPosition;
					dir.Normalize();
					Vector3::TransformNormal(dir, Matrix::CreateFromAxisAngle(dir, 3.14 * 0.5));
					Vector3 yAxis = dir.Cross(cameraPos - mLastPosition);
					yAxis.Normalize();
					topVertex.Position = Vector3::Lerp(position, mLastPosition, ratio) + yAxis * mTrailInfo.Width;
					bottomVertex.Position = Vector3::Lerp(position, mLastPosition, ratio) - yAxis * mTrailInfo.Width;
				}
				break;
				case TrailInfo::EAlignment::TransformZ:
				{
					topVertex.Position = Vector3::Lerp(position, mLastPosition, ratio) + mTrailInfo.Transform.Up() * mTrailInfo.Width;
					bottomVertex.Position = Vector3::Lerp(position, mLastPosition, ratio) - mTrailInfo.Transform.Up() * mTrailInfo.Width;
				}
				break;
				default:
					assert(false);
					break;
				}

				mVertices.push_front(topVertex);
				mVertices.push_front(bottomVertex);
			}

			mLastPosition = position;
		}

		// vertex Á¶Á¤
		while (mVertices.size() > TrailInfo::MAX_VERTEX_SIZE)
		{
			size_t vertexCount = mVertices.size();
			size_t removeCount = (mVertices.size() / 4) * 2;
			vertexCount -= removeCount;

			for (size_t i = 0; i < vertexCount; i += 2)
			{
				mVertices[i] = mVertices[removeCount + i];
				mVertices[i + 1] = mVertices[removeCount + i + 1];
			}

			mVertices.erase(mVertices.begin() + vertexCount, mVertices.end());
		}

		// uv
		if (!mVertices.empty())
		{
			assert(mVertices.size() % 2 == 0);
			switch (mTrailInfo.TextureMode)
			{
			case TrailInfo::ETextureMode::Stretch:

				break;
			case TrailInfo::ETextureMode::Tile:

				break;
			case TrailInfo::ETextureMode::DistributePerSegment:
			{
				float deltaX = 1.f / (mVertices.size() / 2);
				float x = 0.f;

				for (auto iter = mVertices.begin(); iter != mVertices.end(); iter += 2)
				{
					iter->UV = { x, 0 };
					(iter + 1)->UV = { x, 1 };
					x += deltaX;
				}
			}
			break;
			case TrailInfo::ETextureMode::RepeatPerSegment:
			{
				float deltaX = 1;
				float x = 0.f;

				for (auto iter = mVertices.begin(); iter != mVertices.end(); iter += 2)
				{
					iter->UV = { x, 0 };
					(iter + 1)->UV = { x, 1 };
					x += deltaX;
				}
			}
			break;
			default:
				assert(false);
				break;
			}
		}
	}

	void TrailObject::SetTexturePath(const std::wstring& texturePath)
	{
		mTexture = mResourceManager->Create<D3D11Texture>(texturePath);
	}
}