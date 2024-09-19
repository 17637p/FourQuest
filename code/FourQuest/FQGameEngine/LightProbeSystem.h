#pragma once

#include <vector>
#include <directxtk/SimpleMath.h>
#include <string>

namespace fq::graphics
{
	class IProbeObject;
}

namespace fq::game_engine
{
	class GameProcess;

	class LightProbeSystem
	{
	public:
		LightProbeSystem(GameProcess* tempProcess);
		~LightProbeSystem();

		void Finalize();

		// 배치 
		void AddLightProbe(DirectX::SimpleMath::Vector3 position);
		void DeleteLightProbe(fq::graphics::IProbeObject* probeObject);
		void Clear();

		void SaveProbeTexture(bool isAll, int width, int height);
		void BakeLightProbe(bool isAll);

		void SaveLightProbes(std::string fileName);
		void LoadLightProbes(std::string fileName);
		
		float GetLightProbeDefaultScale() { return mLightProbeDefaultScale * 100; }

		// 디버그 드로우 
		std::vector<fq::graphics::IProbeObject*> GetLightProbeObjects() 
		{ 
			return mProbeObjects; 
		};

		float GetLightProbeScale() const { return mLightProbeScale; }
		void SetLightProbeScale(float val) { mLightProbeScale = val; }
	private:
		fq::game_engine::GameProcess* mGameProcess;
		float mLightProbeDefaultScale;
		float mLightProbeScale;

		std::vector<fq::graphics::IProbeObject*> mProbeObjects;
	};
}

// 생성하면 구를 만들어야 함 
// 근데 staticmesh를 가지고 있는 새로운 오브젝트를 만들어야 할듯
// 얘네는 그릴 때 sh만 적용해서 색 그리기 
