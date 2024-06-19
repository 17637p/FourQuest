#include "TrailObject.h"
#include "D3D11ResourceManager.h"

namespace fq::graphics
{
	TrailObject::TrailObject(const DirectX::SimpleMath::Matrix& transform, const TrailInfo& trailInfo, std::shared_ptr<IParticleMaterial> iParticleMaterial)
		: mTransform(transform)
		, mTrailInfo(trailInfo)
		, mIParticleMaterial(iParticleMaterial)
	{
		using namespace DirectX::SimpleMath;
		mLastPosition = Vector3::Transform(Vector3::Zero, mTransform);
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

		DirectX::SimpleMath::Vector3 position = Vector3::Transform(Vector3::Zero, mTransform);

		float distance = DirectX::SimpleMath::Vector3::Distance(position, mLastPosition);
		if (distance > mTrailInfo.MinVertexDistance && mTrailInfo.bIsEmit)
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
					topVertex.Position = Vector3::Lerp(position, mLastPosition, ratio) + mTransform.Up() * mTrailInfo.Width;
					bottomVertex.Position = Vector3::Lerp(position, mLastPosition, ratio) - mTransform.Up() * mTrailInfo.Width;
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

		// vertex 조정
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
			{
				float totalSquaredDistance = 1.f;

				// 엄밀하게 하려면 포지션 값을 저장해야 할듯하네
				for (size_t i = 0; i < mVertices.size() - 2; i += 2)
				{
					totalSquaredDistance += Vector3::Distance(mVertices[i].Position, mVertices[i + 2].Position);
				}
				float remainder = 0.f;

				for (size_t i = 0; i < mVertices.size() - 2; i += 2)
				{
					float texcoordX = Vector3::Distance(mVertices[i].Position, mVertices[i + 2].Position) / totalSquaredDistance;

					mVertices[i].UV = { remainder, 0 };
					mVertices[i + 1].UV = { remainder, 1 };
				
					remainder += texcoordX;
				}

				mVertices[mVertices.size() - 2].UV = { 1, 0 };
				mVertices[mVertices.size() - 1].UV = { 1, 1 };
			}
			break;
			case TrailInfo::ETextureMode::Tile:
			{
				// 현재에서 다음 프레임 누적해가면서 10으로 나눠주는 형태?
				float remainder = 0.f;

				for (size_t i = 0; i < mVertices.size() - 2; i += 2)
				{
					float texcoordX = Vector3::Distance(mVertices[i].Position, mVertices[i + 2].Position) / 10.f;

					mVertices[i].UV = { remainder, 0 };
					mVertices[i + 1].UV = { remainder, 1 };

					remainder += texcoordX;
				}

				mVertices[mVertices.size() - 2].UV = { remainder, 0 };
				mVertices[mVertices.size() - 1].UV = { remainder, 1 };
			}
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
}