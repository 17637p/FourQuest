#include "SoundEmitter.h"

#include "EventManager.h"
#include "Scene.h"
#include "Event.h"
#include "GameModule.h"

fq::game_module::SoundEmitter::~SoundEmitter()
{
}

fq::game_module::SoundEmitter::SoundEmitter()
	: mbIsPlayLoop(false)
	, mbIsPlayStateEnter(false)
	, mSoundTurm(0.5f)
	, mbUseRandomPlay(false)
	, mSoundNames{}

	, mElapsedTime(0.f)
	, mSoundIndex(0)
	, mbIsPlaySoundOnce(false)
{

}

entt::meta_handle fq::game_module::SoundEmitter::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::SoundEmitter::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSoundEmitter = std::dynamic_pointer_cast<SoundEmitter>(clone);

	if (cloneSoundEmitter == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneSoundEmitter = ObjectPool::GetInstance()->Assign<SoundEmitter>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSoundEmitter = *this;
	}

	return cloneSoundEmitter;
}

void fq::game_module::SoundEmitter::OnStart()
{
	mElapsedTime = 0.f;
	mSoundIndex = 0u;
	mbIsPlaySoundOnce = false;

	if (mbIsPlayStateEnter)
	{
		playSound();
	}
}

void fq::game_module::SoundEmitter::OnUpdate(float dt)
{
	if (!mbIsPlayLoop && mbIsPlaySoundOnce)
	{
		return;
	}

	mElapsedTime += dt;

	if (mElapsedTime >= mSoundTurm)
	{
		mElapsedTime -= mSoundTurm;
		mbIsPlaySoundOnce = true;
		playSound();
	}
}

void fq::game_module::SoundEmitter::playSound()
{
	if (mbUseRandomPlay)
	{
		mSoundIndex = std::rand() % mSoundNames.size();
	}
	else
	{
		mSoundIndex = (mSoundIndex + 1) % mSoundNames.size();
	}

	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ mSoundNames[mSoundIndex], false , fq::sound::EChannel::SE });
}