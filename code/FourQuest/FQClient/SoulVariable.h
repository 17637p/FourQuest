#pragma once

#include "IGameVariable.h"

namespace fq::client
{
	class SoulVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//							��ȥ ��Ż �� �ı�								//
		//////////////////////////////////////////////////////////////////////////
		static inline float ButtonTime = 0.5f;		// ��ȥ ��Ż �ð�
		static inline float OutTime = 3.f;			// ���� �ı� ��, ���� �ƿ� �ð�
		static inline int HpPercent = 30;			// ���� �԰� �ִ� ���¿��� ��Ż�� �ÿ� ���� ������ �� �ִ� �ּ� HP

		//////////////////////////////////////////////////////////////////////////
		//								��ȥ ü��									//
		//////////////////////////////////////////////////////////////////////////
		static inline int SoulMaxHp = 100;				// �ҿ��� HP �ִ�ġ
		static inline int SoulMinHp = 40;				// ��� �� ��ȥ �ִ� ü���� �پ��µ�, ���� ��� Ƚ�� �̻󿡼��� �ּ� HP���� �����ϸ� �ִ� HP���� �پ���� �ʴ� ����
		static inline int SoulHpDown = 20;				// ��ȥ �ı� ��, �ִ� ü�� ���ҷ�
		static inline float SoulHpDecreas = 5.f;		// �ʴ� HP�� �����ϴ� ��
		static inline float SoulDistance = 5.f;			// �� �����Ÿ� �ȿ� ������ ���� �÷��̾ ���� ��� ���ط� ����
		static inline int SoulDecreasPercentage = 50;	// �����Ÿ� �ȿ� ���� �÷��̾ ���� ��, HP ���� ���ҷ�

		//////////////////////////////////////////////////////////////////////////
		//								�÷��̾� ��ȭ								//
		//////////////////////////////////////////////////////////////////////////
		static inline float SpeedUpRatio = 1.2f;				// �÷��̾� �̵��ӵ� ������
		static inline float DamageUpRatio = 1.2f;			// �÷��̾� ���ݷ� ������
	};
}