#include "Sequence.h"

#include "..\FQClient\Player.h"
#include "Track.h"

namespace fq::game_module
{
	Sequence::Sequence()
		: mTracks()
		, mbIsPlay(false)
		, mbIsLoop(false)
		, mTotalPlayTime(0.f)
		, mDurationTime(0.f)
	{
	}

	Sequence::~Sequence()
	{
	}

	void Sequence::OnUpdate(float dt)
	{
		if (mbIsPlay)
		{
			mDurationTime += dt;

			for (const auto& track : mTracks)
			{
				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;

				track->Play(mDurationTime);
			}


			if (mDurationTime >= mTotalPlayTime)
			{
				mDurationTime = 0;

				if (!mbIsLoop)
					mbIsPlay = false;
			}
		}
	}

	void Sequence::OnTriggerEnter(const Collision& collision)
	{
		if (collision.object->HasComponent<fq::client::Player>())
		{
			mbIsPlay = true;
		}
	}

	entt::meta_handle Sequence::GetHandle()
	{
		return *this;
	}

	std::shared_ptr<Component> Sequence::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneSequence = std::dynamic_pointer_cast<Sequence>(clone);

		if (cloneSequence == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneSequence = ObjectPool::GetInstance()->Assign<Sequence>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneSequence = *this;
		}

		return cloneSequence;
	}
}
