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

		// ��ġ 
		void AddLightProbe(DirectX::SimpleMath::Vector3 position);
		void DeleteLightProbe(fq::graphics::IProbeObject* probeObject);
		void Clear();

		void SaveProbeTexture(int width, int height);
		void BakeLightProbe();

		void SaveLightProbes(std::string fileName);
		void LoadLightProbes(std::string fileName);
		
		float GetLightProbeScale() { return mLightProbeScale * 100; }

		// ����� ��ο� 
		std::vector<fq::graphics::IProbeObject*> GetLightProbeObjects() { return mProbeObjects; };

	private:
		fq::game_engine::GameProcess* mGameProcess;
		float mLightProbeScale;

		std::vector<fq::graphics::IProbeObject*> mProbeObjects;
	};
}

// �����ϸ� ���� ������ �� 
// �ٵ� staticmesh�� ������ �ִ� ���ο� ������Ʈ�� ������ �ҵ�
// ��״� �׸� �� sh�� �����ؼ� �� �׸��� 
