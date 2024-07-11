#pragma once
#include "Component.h"
#include "ICollider.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	using ArticulationInfo = fq::physics::ArticulationInfo;
	using ArticulationPath = std::string;

	class Articulation : public Component, public ICollider
	{
	public:
		Articulation();
		~Articulation();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// �ε��� ������ ��θ� ��ȯ�մϴ�.
		/// </summary>
		ArticulationPath GetArticulationPath() const { return mArticulationPath; }

		/// <summary>
		/// �ε��� ������ ��θ� �����մϴ�.
		/// </summary>
		void SetArticulationPath(ArticulationPath path) { mArticulationPath = path; }

		/// <summary>
		/// ������ ���̵� ��ȯ�մϴ�.
		/// </summary>
		unsigned int GetID() const { return mID; }

		/// <summary>
		/// ������ ���̵�  �����մϴ�.
		/// </summary>
		void SetArticulationID(unsigned int id) { mID = id; }

		/// <summary>
		/// ���� �浹���� �ݶ��̴��� ������ ��ȯ�մϴ� 
		/// </summary>
		unsigned int GetCollisionCount() const { return mCollisionCount; }

		/// <summary>
		/// �ݶ��̴��� �������� ��ȯ�մϴ�.
		/// </summary>
		DirectX::SimpleMath::Vector3 GetOffset() const override { return mOffset; }

		/// <summary>
		/// �ݶ��̴��� �������� �����մϴ�.
		/// </summary>
		void SetOffset(DirectX::SimpleMath::Vector3 offset)override { mOffset = offset; }

	private:
		inline void OnCollisionEnter(const Collision& collision) override;
		inline void OnCollisionExit(const Collision& collision) override;
		inline void OnTriggerEnter(const Collision& collision) override;
		inline void OnTriggerExit(const Collision& collision) override;

		inline entt::meta_handle GetHandle() override;

	private:
		unsigned int mID;
		ArticulationPath mArticulationPath;

		DirectX::SimpleMath::Vector3 mOffset;

		unsigned int mCollisionCount;
	};
	
	void Articulation::OnCollisionEnter(const Collision& collision)
	{
		mCollisionCount++;
	}
	void Articulation::OnCollisionExit(const Collision& collision)
	{
		assert(mCollisionCount != 0);
		mCollisionCount--;
	}
	void Articulation::OnTriggerEnter(const Collision& collision)
	{
		mCollisionCount++;
	}
	void Articulation::OnTriggerExit(const Collision& collision)
	{
		assert(mCollisionCount != 0);
		mCollisionCount--;
	}
}