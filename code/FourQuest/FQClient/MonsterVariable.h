#pragma once
#include "IGameVariable.h"

namespace fq::client
{
	class MonsterVariable : public IGameVariable
	{
	public:
		static inline bool OnRagdoll = true;						// ���׵��� ������ �� ����
		static inline int MaxOneFrameCreateRagdollCount = 3;		// �� �����ӿ� ���׵� �ִ� ���� ����
		static inline int MaxRagdollsPerScene = 15;					// �� ���� ���׵� �ִ� ���� ����
		static inline int MinFrameCountForRagdoll = 144;			// ���׵��� ������ �ּ� ������ ����
	}; 
}
