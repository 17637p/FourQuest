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

// �ϴ� Delete�� �����ϴµ� ���߿� �̸� �����ΰ� enum���� �������� �ؾ��ҵ�
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
	// Todo: ���� �̸��� State�� �̹� �ִٸ� ��� �޽��� �߰�
	mStates[stateName] = state;
}

void fq::client::MonsterFSM::DeleteState(const std::string stateName)
{
	mStates.erase(stateName);
}

