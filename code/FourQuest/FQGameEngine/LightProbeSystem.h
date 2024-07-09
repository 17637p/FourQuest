#pragma once

#include <vector>
#include <directxtk/SimpleMath.h>

namespace fq::game_engine
{
	class GameProcess;

	struct LightProbe
	{
		int index;
		DirectX::SimpleMath::Vector3 position;
	};

	class LightProbeSystem
	{
	public:
		LightProbeSystem(GameProcess* tempProcess);
		~LightProbeSystem();

		// ��ġ 
		void AddLightProbe(DirectX::SimpleMath::Vector3 position);
		void DeleteLightProbe(int index);
		// ����� ����� 
		

		// ����� ��ο� 
		std::vector<LightProbe*> GetLightProbes() { return mLightProbes; }

	private:
		fq::game_engine::GameProcess* mGameProcess;

		std::vector<LightProbe*> mLightProbes;
	};
}

// �����ϸ� ���� ������ �� 
// �ٵ� staticmesh�� ������ �ִ� ���ο� ������Ʈ�� ������ �ҵ�
// ��״� �׸� �� sh�� �����ؼ� �� �׸��� 
