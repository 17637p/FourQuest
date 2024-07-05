#include "TrailObject.h"
#include "D3D11ResourceManager.h"

namespace fq::graphics
{
	TrailObject::TrailObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const TrailInfo& trailInfo, const DirectX::SimpleMath::Matrix& transform)
		: mIParticleMaterial(iParticleMaterial)
		, mTrailInfo(trailInfo)
		, mTransform(transform)
		, mFrameTime(0.f)
	{
		using namespace DirectX::SimpleMath;
		mLastPosition = Vector3::Transform(Vector3::Zero, mTransform);
	}

	void TrailObject::Simulate(DirectX::SimpleMath::Vector3 cameraPos)
	{
		using namespace DirectX::SimpleMath;

		if (mTrailInfo.bIsReset)
		{
			mTrailInfo.bIsReset = false;

			mVertices.clear();
			mCatmulRomTopVertics.clear();
			mCatmulRomBottomVertics.clear();
		}

		auto iter1 = mCatmulRomTopVertics.begin();
		for (; iter1 != mCatmulRomTopVertics.end(); ++iter1)
		{
			if (iter1->Age >= mTrailInfo.Time)
			{
				break;
			}

			iter1->Age += mFrameTime;
		}
		mCatmulRomTopVertics.erase(iter1, mCatmulRomTopVertics.end());

		auto iter2 = mCatmulRomBottomVertics.begin();
		for (; iter2 != mCatmulRomBottomVertics.end(); ++iter2)
		{
			if (iter2->Age >= mTrailInfo.Time)
			{
				break;
			}

			iter2->Age += mFrameTime;
		}
		mCatmulRomBottomVertics.erase(iter2, mCatmulRomBottomVertics.end());

		auto iter = mVertices.begin();
		for (; iter != mVertices.end(); ++iter)
		{
			if (iter->Age >= mTrailInfo.Time)
			{
				break;
			}

			iter->Age += mFrameTime;

		}
		mVertices.erase(iter, mVertices.end());

		DirectX::SimpleMath::Vector3 position = Vector3::Transform(Vector3::Zero, mTransform);
		float distance = DirectX::SimpleMath::Vector3::Distance(position, mLastPosition);

		if (!mTrailInfo.bIsEmit)
		{
			mLastPosition = position;
		}
		else
		{
			if (distance > mTrailInfo.MinVertexDistance && mTrailInfo.bIsEmit)
			{
				size_t frameMaxDivideCount = 1.f; // std::min<size_t>(distance / mTrailInfo.MinVertexDistance, TrailInfo::MAX_VERTEX_SIZE / 60.f);

				for (size_t i = 0; i < frameMaxDivideCount; ++i)
				{
					float ratio = (float)(i + 1) / (frameMaxDivideCount);

					Vertex topVertex;
					Vertex bottomVertex;
					topVertex.Age = 0;
					bottomVertex.Age = 0;

					switch (mTrailInfo.AlignmentType)
					{
					case TrailInfo::EAlignment::View:
					{
						Vector3 xAxis = position - mLastPosition;
						xAxis.Normalize();
						Vector3 zAxis = cameraPos - mLastPosition;
						zAxis.Normalize();
						Vector3 yAxis = zAxis.Cross(xAxis);
						yAxis.Normalize();

						topVertex.Origin = Vector3::Lerp(mLastPosition, position, ratio);
						topVertex.ExtendAxis = yAxis;
						bottomVertex.Origin = topVertex.Origin;
						bottomVertex.ExtendAxis = -yAxis;
					}
					break;
					case TrailInfo::EAlignment::TransformZ:
					{
						topVertex.Origin = Vector3::Lerp(mLastPosition, position, ratio);
						topVertex.ExtendAxis = mTransform.Up();
						bottomVertex.Origin = topVertex.Origin;
						bottomVertex.ExtendAxis = -mTransform.Up();
					}
					break;
					default:
						assert(false);
						break;
					}

					mVertices.push_front(topVertex); // 앞에 있을 수록 최신 데이터, 
					mCatmulRomTopVertics.push_front(topVertex);
					mVertices.push_front(bottomVertex);
					mCatmulRomBottomVertics.push_front(bottomVertex);

					while (mCatmulRomTopVertics.size() >= 4)
					{
						for (size_t j = 0; j < mTrailInfo.VertexDivisionCount; ++j)
						{
							Vertex catmullRomTopVertex;
							Vertex catmullRomBottomVertex;

							float weight = (float)(j + 1) / (mTrailInfo.VertexDivisionCount + 1);

							catmullRomTopVertex.ExtendAxis = DirectX::SimpleMath::Vector3::Lerp(mCatmulRomTopVertics[2].ExtendAxis, mCatmulRomTopVertics[1].ExtendAxis, weight);
							catmullRomTopVertex.Age = std::lerp(mCatmulRomTopVertics[2].Age, mCatmulRomTopVertics[1].Age, weight);
							catmullRomTopVertex.Origin = catmullRom(
								mCatmulRomTopVertics[3].Origin,
								mCatmulRomTopVertics[2].Origin,
								mCatmulRomTopVertics[1].Origin,
								mCatmulRomTopVertics[0].Origin,
								weight);

							catmullRomBottomVertex.ExtendAxis = DirectX::SimpleMath::Vector3::Lerp(mCatmulRomBottomVertics[2].ExtendAxis, mCatmulRomBottomVertics[1].ExtendAxis, weight);
							catmullRomBottomVertex.Age = std::lerp(mCatmulRomBottomVertics[2].Age, mCatmulRomBottomVertics[1].Age, weight);
							catmullRomBottomVertex.Origin = catmullRom(
								mCatmulRomBottomVertics[3].Origin,
								mCatmulRomBottomVertics[2].Origin,
								mCatmulRomBottomVertics[1].Origin,
								mCatmulRomBottomVertics[0].Origin,
								weight);

							mVertices.insert(mVertices.begin() + 4, catmullRomTopVertex);
							mVertices.insert(mVertices.begin() + 4, catmullRomBottomVertex);
						}

						mCatmulRomTopVertics.pop_back();
						mCatmulRomBottomVertics.pop_back();
					}
				}

				mLastPosition = position;
			}
		}

		// vertex 조정
		while (mVertices.size() > TrailInfo::MAX_VERTEX_SIZE)
		{
			size_t removeCount = (mVertices.size() / 4) * 2;
			mVertices.erase(mVertices.end() - (removeCount), mVertices.end());
		}

		// 보간 처리
		size_t i = 0;

		for (auto& vertex : mVertices)
		{
			float lifetimeRatio = vertex.Age / mTrailInfo.Time;

			// extend
			if (mTrailInfo.AlignmentType == TrailInfo::EAlignment::View && mVertices.size() > i + 2)
			{
				const auto& nextVertex = mVertices[i + 2];
				Vector3 xAxis = nextVertex.Origin - vertex.Origin;
				xAxis.Normalize();
				Vector3 zAxis = cameraPos - vertex.Origin;
				zAxis.Normalize();
				Vector3 yAxis = zAxis.Cross(xAxis);
				yAxis.Normalize();

				vertex.ExtendAxis = i % 2 == 0 ? yAxis : -yAxis;
			}
			++i;

			// pos, gpu에서 처리 가능
			if (!mTrailInfo.WidthRatios.empty())
			{
				auto next = mTrailInfo.WidthRatios.begin();

				for (; next != mTrailInfo.WidthRatios.end(); ++next)
				{
					if (next->y > lifetimeRatio)
					{
						break;
					}
				}

				if (next == mTrailInfo.WidthRatios.begin())
				{
					vertex.Position = vertex.Origin + vertex.ExtendAxis * next->x;
				}
				else if (next == mTrailInfo.WidthRatios.end())
				{
					auto prev = std::prev(next);
					vertex.Position = vertex.Origin + vertex.ExtendAxis * prev->x;
				}
				else
				{
					auto prev = std::prev(next);
					float devider = (next->y - prev->y);
					float lerpRatio = (lifetimeRatio - prev->y) / (devider == 0 ? 0.001f : devider);
					vertex.Position = vertex.Origin + vertex.ExtendAxis * std::lerp(prev->x, next->x, lerpRatio);
				}
			}

			// color, gpu에서 처리 가능
			if (!mTrailInfo.ColorRatios.empty())
			{
				auto next = mTrailInfo.ColorRatios.begin();

				for (; next != mTrailInfo.ColorRatios.end(); ++next)
				{
					if (next->w > lifetimeRatio)
					{
						break;
					}
				}

				if (next == mTrailInfo.ColorRatios.begin())
				{
					vertex.Color.x = next->x;
					vertex.Color.y = next->y;
					vertex.Color.z = next->z;
				}
				else if (next == mTrailInfo.ColorRatios.end())
				{
					auto prev = std::prev(next);
					vertex.Color.x = prev->x;
					vertex.Color.y = prev->y;
					vertex.Color.z = prev->z;
				}
				else
				{
					auto prev = std::prev(next);
					float devider = (next->w - prev->w);
					float lerpRatio = (lifetimeRatio - prev->w) / (devider == 0 ? 0.001f : devider);
					vertex.Color.x = std::lerp(prev->x, next->x, lerpRatio);
					vertex.Color.y = std::lerp(prev->y, next->y, lerpRatio);
					vertex.Color.z = std::lerp(prev->z, next->z, lerpRatio);
				}
			}

			// alpha, gpu에서 처리 가능
			if (!mTrailInfo.AlphaRatios.empty())
			{
				auto next = mTrailInfo.AlphaRatios.begin();

				for (; next != mTrailInfo.AlphaRatios.end(); ++next)
				{
					if (next->y > lifetimeRatio)
					{
						break;
					}
				}

				if (next == mTrailInfo.AlphaRatios.begin())
				{
					vertex.Color.w = next->x;
				}
				else if (next == mTrailInfo.AlphaRatios.end())
				{
					auto prev = std::prev(next);
					vertex.Color.w = prev->x;
				}
				else
				{
					auto prev = std::prev(next);
					float devider = (next->y - prev->y);
					float lerpRatio = (lifetimeRatio - prev->y) / (devider == 0 ? 0.001f : devider);
					vertex.Color.w = std::lerp(prev->x, next->x, lerpRatio);
				}
			}
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

	DirectX::SimpleMath::Vector3 TrailObject::catmullRom(const DirectX::SimpleMath::Vector3& P0, const DirectX::SimpleMath::Vector3& P1, const DirectX::SimpleMath::Vector3& P2, const DirectX::SimpleMath::Vector3& P3, float t)
	{
		float t2 = t * t;
		float t3 = t2 * t;

		DirectX::SimpleMath::Vector3 result;
		result = 0.5f * ((2.0f * P1) +
			(-P0 + P2) * t +
			(2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * t2 +
			(-P0 + 3.0f * P1 - 3.0f * P2 + P3) * t3);

		return result;
	}
}