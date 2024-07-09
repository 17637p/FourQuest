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

		// 배치 
		void AddLightProbe(DirectX::SimpleMath::Vector3 position);
		void DeleteLightProbe(int index);
		// 기즈모 써야함 
		

		// 디버그 드로우 
		std::vector<LightProbe*> GetLightProbes() { return mLightProbes; }

	private:
		fq::game_engine::GameProcess* mGameProcess;

		std::vector<LightProbe*> mLightProbes;
	};
}

// 생성하면 구를 만들어야 함 
// 근데 staticmesh를 가지고 있는 새로운 오브젝트를 만들어야 할듯
// 얘네는 그릴 때 sh만 적용해서 색 그리기 
