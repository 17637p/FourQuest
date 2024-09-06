#pragma once
#include "IGameVariable.h"

namespace fq::client
{
	class PlayerInfoVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//					    �� ��ȯ�Ǿ ������ ����							//
		//////////////////////////////////////////////////////////////////////////

		// �÷��̾� ��ȥ ���� ����
		static inline int Player1SoulType = -1;
		static inline int Player2SoulType = -1;
		static inline int Player3SoulType = -1;
		static inline int Player4SoulType = -1;

		// �÷��̾� ���� HP
		static inline float Player1HP = 100.f;
		static inline float Player2HP = 100.f;
		static inline float Player3HP = 100.f;
		static inline float Player4HP = 100.f;

		// �÷��̾� SoulGauge
		static inline float Player1SoulGauge = 100.f;
		static inline float Player2SoulGauge = 100.f;
		static inline float Player3SoulGauge = 100.f;
		static inline float Player4SoulGauge = 100.f;

		/// ��� ȭ�� ��
		// ���� ��� �� 
		static inline int Player1Knight = 0;
		static inline int Player2Knight = 0;
		static inline int Player3Knight = 0;
		static inline int Player4Knight = 0;

		static inline int Player1Magic = 0;
		static inline int Player2Magic = 0;
		static inline int Player3Magic = 0;
		static inline int Player4Magic = 0;

		static inline int Player1Warrior = 0;
		static inline int Player2Warrior = 0;
		static inline int Player3Warrior = 0;
		static inline int Player4Warrior = 0;

		static inline int Player1Archer = 0;
		static inline int Player2Archer = 0;
		static inline int Player3Archer = 0;
		static inline int Player4Archer = 0;

		// óġ�� ���� ��
		static inline int Player1Monster = 0;
		static inline int Player2Monster = 0;
		static inline int Player3Monster = 0;
		static inline int Player4Monster = 0;
	};
}
