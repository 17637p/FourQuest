#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	using ClothPath = std::string;
	using ClothInfo = fq::physics::Cloth::CreateClothData;

	class ClothCollider : public Component
	{
	public:
		ClothCollider();
		~ClothCollider();

		ClothPath GetClothPath() const { return mClothPath; }
		float GetClothMass() const { return mClothInfo.clothMass; }
		float GetRestOffset() const { return mClothInfo.restOffset; }
		float GetFriction() const { return mClothInfo.materialInfo.friction; }
		float GetDamping() const { return mClothInfo.materialInfo.damping; }
		float GetAdhesion() const { return mClothInfo.materialInfo.adhesion; }
		float GetViscosity() const { return mClothInfo.materialInfo.viscosity; }
		float GetVorticityConfinement() const { return mClothInfo.materialInfo.vorticityConfinement; }
		float GetSurfaceTension() const { return mClothInfo.materialInfo.surfaceTension; }
		float GetCohesion() const { return mClothInfo.materialInfo.cohesion; }
		float GetLift() const { return mClothInfo.materialInfo.lift; }
		float GetDrag() const { return mClothInfo.materialInfo.drag; }
		float GetCflCoefficient() const { return mClothInfo.materialInfo.cflCoefficient; }
		float GetGravityScale() const { return mClothInfo.materialInfo.gravityScale; }
		void SetClothPath(ClothPath path) { mClothPath = path; }
		void SetClothMass(float clothMass) { mClothInfo.clothMass = clothMass; }
		void SetRestOffset(float restOffset) { mClothInfo.restOffset = restOffset; }
		void SetFriction(float friction) { mClothInfo.materialInfo.friction = friction; }
		void SetDamping	(float damping) { mClothInfo.materialInfo.damping = damping; }
		void SetAdhesion(float adhesion) { mClothInfo.materialInfo.adhesion = adhesion; }
		void SetViscosity(float viscosity) { mClothInfo.materialInfo.viscosity = viscosity; }
		void SetVorticityConfinement(float vorticityConfinement) { mClothInfo.materialInfo.vorticityConfinement = vorticityConfinement; }
		void SetSurfaceTension(float surfaceTension) { mClothInfo.materialInfo.surfaceTension = surfaceTension; }
		void SetCohesion(float cohesion) { mClothInfo.materialInfo.cohesion = cohesion; }
		void SetLift(float lift) { mClothInfo.materialInfo.lift = lift; }
		void SetDrag(float drag) { mClothInfo.materialInfo.drag = drag; }
		void SetCflCoefficient(float cflCoefficient) { mClothInfo.materialInfo.cflCoefficient = cflCoefficient; }
		void SetGravityScale(float gravityScale) { mClothInfo.materialInfo.gravityScale = gravityScale; }


	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override;

	private:
		ClothPath mClothPath;
		ClothInfo mClothInfo;

		float mClothMass;
		float mRestOffset;
	};
}

