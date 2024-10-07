#include "StateEventEmitter.h"

#include "EventManager.h"
#include "Scene.h"
#include "Event.h"
#include "GameModule.h"

fq::game_module::StateEventEmitter::~StateEventEmitter()
{
}

fq::game_module::StateEventEmitter::StateEventEmitter()
	: mbIsPlayLoop(false)
	, mbIsPlayStateEnter(false)
	, mTurm(0.5f)
	, mbUseRandomIndex(false)
	, mNames{}

	, mElapsedTime(0.f)
	, mNameIndex(0)
	, mbIsPlayOnce(false)
{

}

entt::meta_handle fq::game_module::StateEventEmitter::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::StateEventEmitter::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneStateEventEmitter = std::dynamic_pointer_cast<StateEventEmitter>(clone);

	if (cloneStateEventEmitter == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneStateEventEmitter = ObjectPool::GetInstance()->Assign<StateEventEmitter>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneStateEventEmitter = *this;
	}

	return cloneStateEventEmitter;
}

void fq::game_module::StateEventEmitter::OnStart()
{
	mElapsedTime = 0.f;
	mNameIndex = 0u;
	mbIsPlayOnce = false;

	if (mbIsPlayStateEnter)
	{
		emit();
	}
}

void fq::game_module::StateEventEmitter::OnUpdate(float dt)
{
	if (!mbIsPlayLoop && mbIsPlayOnce)
	{
		return;
	}

	mElapsedTime += dt;

	if (mElapsedTime >= mTurm)
	{
		mElapsedTime -= mTurm;
		mbIsPlayOnce = true;
		emit();
	}
}

void fq::game_module::StateEventEmitter::emit()
{
	if (mNames.empty())
	{
		return;
	}

	if (mbUseRandomIndex)
	{
		mNameIndex = std::rand() % mNames.size();
	}
	else
	{
		mNameIndex = (mNameIndex + 1) % mNames.size();
	}

	fq::event::OnCreateStateEvent stateEvent;
	stateEvent.gameObject = GetGameObject();
	stateEvent.RegisterKeyName = mNames[mNameIndex];
	GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
}