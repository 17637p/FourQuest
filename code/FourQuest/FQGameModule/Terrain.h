#pragma once
#include "Component.h"
#include "../FQCommon/FQCommonGraphics.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics { class ITerrainMeshObject; }

namespace fq::game_module
{
	class Terrain : public Component
	{
	public:
		Terrain();
		virtual ~Terrain();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void SetTerrainLayers(const std::vector<fq::graphics::TerrainLayer>& terrainLayers);
		std::vector<fq::graphics::TerrainLayer> GetTerrainLayers() const;

		void SetAlphaMap(const std::string& alphaMapPath);
		std::string GetAlphaMap() const;

		void SetTerrainMeshObject(fq::graphics::ITerrainMeshObject* terrainMeshObject);
		fq::graphics::ITerrainMeshObject* GetTerrainMeshObject() const;

	private:
		entt::meta_handle GetHandle() override;

	private:
		fq::graphics::ITerrainMeshObject* mTerrainMeshObject;

		std::vector<fq::graphics::TerrainLayer> mTerrainLayers; // 최대 4
		std::string mAlPhaMapPath; // R에는 BaseColor1, G에는 2, B에는 3, A에는 4
	};


}