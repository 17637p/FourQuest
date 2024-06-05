#include "MonsterFSM.h"
#include "MonsterBaseState.h"

fq::client::MonsterFSM::MonsterFSM()
	:mCurState("")
{
}

fq::client::MonsterFSM::~MonsterFSM()
{

}

void fq::client::MonsterFSM::OnStart(const std::string& baseState)
{
	OnChangeState(baseState);
}

// 일단 Delete로 구현하는데 나중에 미리 만들어두고 enum으로 꺼내쓰든 해야할듯
void fq::client::MonsterFSM::OnChangeState(const std::string& nextState)
{
	if (nextState == mCurState)
	{
		return;
	}

	if (mCurState != "")
	{
		mStates.at(mCurState)->OnStateExit();
	}

	mCurState = nextState;
	mStates.at(mCurState)->OnStateEnter();
}

void fq::client::MonsterFSM::OnUpdate(float deltaTime)
{
	if (mCurState != "")
	{
		mStates.at(mCurState)->OnStateUpdate(deltaTime);
	}
}

void fq::client::MonsterFSM::RegisterState(const std::string stateName, const std::shared_ptr<MonsterBaseState>& state)
{
	// Todo: 같은 이름의 State가 이미 있다면 경고 메시지 추가
	mStates[stateName] = state;
}

void fq::client::MonsterFSM::DeleteState(const std::string stateName)
{
	mStates.erase(stateName);
}

