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
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 로드할 관절의 경로를 반환합니다.
		/// </summary>
		ArticulationPath GetArticulationPath() const { return mArticulationPath; }

		/// <summary>
		/// 로드할 관절의 경로를 설정합니다.
		/// </summary>
		void SetArticulationPath(ArticulationPath path) { mArticulationPath = path; }

		/// <summary>
		/// 관절의 아이디를 반환합니다.
		/// </summary>
		unsigned int GetID() const { return mID; }

		/// <summary>
		/// 관절의 아이디를  설정합니다.
		/// </summary>
		void SetArticulationID(unsigned int id) { mID = id; }

		/// <summary>
		/// 현재 충돌중이 콜라이더의 갯수를 반환합니다 
		/// </summary>
		unsigned int GetCollisionCount() const { return mCollisionCount; }

		/// <summary>
		/// 콜라이더의 오프셋을 반환합니다.
		/// </summary>
		DirectX::SimpleMath::Vector3 GetOffset() const override { return mOffset; }

		/// <summary>
		/// 콜라이더의 오프셋을 설정합니다.
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