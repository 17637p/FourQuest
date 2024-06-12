#pragma once

#include "../FQGameModule/GameModule.h"

class CreateNavigationMesh : public fq::game_module::Component
{
	class NavigationMeshData;
public:
	struct BuildSettings
	{
		// ��ã�� ��ü���� �ִ� ��ü��
		int maxCrowdNumber{ 1024 };
		// ��ã�� ��ü���� �ִ� �浹�ݰ�
		float maxAgentRadius{ 0.6 };
		// ������ �ִ� ���
		float walkableSlopeAngle{ 30 };
		// ���� �� �ִ� ����
		float walkableClimb{ 0.2 };
		// õ���� �ּ� ����
		float walkableHeight{ 0.3 };
		// x��,z�� ���� ������ ����, ������ �������� ������ �� ������������, �׺���̼� ����� ��ã�� �ý����� ���ϰ� �ð� �ȴ�.
		float divisionSizeXZ{ 0.3 };
		// y�� ���� ������ ����, ������ �������� ������ �� ������������, �׺���̼� ����� ��ã�� �ý����� ���ϰ� �ð� �ȴ�.
		float divisionSizeY{ 0.2 };
		// ���� ������ xz������ 250*330, y������ 200�� ���� ���ҵǴ� ������ ���İ��� �׺���̼� ���嵵 �Ǹ鼭 ��ã�⵵ �������� �ϰ� �Ǵ� ������.
		// xz������ 743* 989�� ���� ������ �ǵ� ū ���ϴ� ����.
	};

	CreateNavigationMesh();
	virtual ~CreateNavigationMesh();

	virtual void OnStart() override;

	void BuildField(std::vector<DirectX::SimpleMath::Vector3> worldVertices, std::vector<int> faces, const BuildSettings& buildSettings = BuildSettings{});

	bool GetIsBuildMesh() const { return mIsBuildMesh; }
	void SetIsBuildMesh(bool val) { mIsBuildMesh = val; }

	void DebugDraw();

private:
	void buildField(
		const float* worldVertices, size_t verticesNum, 
		const int* faces, size_t facesNum, 
		const BuildSettings& buildSettings = BuildSettings{});

private:
	bool mIsBuildMesh;
	NavigationMeshData* impl{ nullptr };
	//friend NavigationAgent;
};