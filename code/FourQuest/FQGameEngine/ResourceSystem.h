#pragma once

#include <shared_mutex>
#include <mutex>

#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/EventHandler.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/AnimatorControllerLoader.h"
#include "SceneResourceList.h"

namespace fq::game_engine
{
	class GameProcess;

	class ResourceSystem
	{
		using Path = std::string;
		using Mutex = std::shared_mutex;
		using EventHandler = fq::game_module::EventHandler;

	public:
		ResourceSystem();
		~ResourceSystem();

		void Finalize();
		void Initialize(GameProcess* game);

		void SaveSceneResourceList(const std::filesystem::path& path);
		void SaveObjectResource(SceneResourceList& list, game_module::GameObject* object);
		void SaveAnimationResource(SceneResourceList& list);

		const fq::common::Model& GetModel(const Path& path)const;

		void LoadSceneResource(fq::event::PreOnLoadScene event);

		std::shared_ptr<graphics::IStaticMesh> GetStaticMesh(const Path& modelPath, std::string meshName) const;
		std::shared_ptr<graphics::ISkinnedMesh> GetSkinnedMesh(const Path& modelPath, std::string meshName) const;
		std::shared_ptr<graphics::IMaterial> GetMaterial(const Path& path) const;
		std::shared_ptr<graphics::IAnimation> GetAnimation(const Path& path) const;
		std::shared_ptr<graphics::IUVAnimation> GetUVAnimation(const Path& path) const;
		std::shared_ptr<graphics::INodeHierarchy> GetNodeHierarchy(const Path& path) const;

		bool HasModel(const Path& path) const;
		bool HasAnimation(const Path& path) const;

		void LoadModelResource(const Path& path);
		void LoadAnimation(const Path& path);
		void LoadMaterial(const Path& path);
		void LoadUVAnimation(const Path& path);
		void LoadNodeHierarchy(const Path& path);

		inline std::map<Path, std::shared_ptr<graphics::IMaterial>> GetMaterials() const; // 그레이 스케일로 인해 추가

		/// <summary>
		/// 현재 로드된 리소스 비율을 반환합니다.
		/// </summary>
		float GetLoadingRatio()const;

	private:
		GameProcess* mGameProcess;
		EventHandler mPreOnLoadSceneHandler;
		fq::game_module::AnimatorControllerLoader mLoader;

		std::map<Path, fq::common::Model> mModels;
		std::map<Path, std::shared_ptr<graphics::IStaticMesh>> mStaticMeshes;
		std::map<Path, std::shared_ptr<graphics::ISkinnedMesh>> mSkinnedMeshes;
		std::map<Path, std::shared_ptr<graphics::INodeHierarchy>> mNodeHierarchies;
		std::map<Path, std::shared_ptr<graphics::IAnimation>> mAnimations;
		std::map<Path, std::shared_ptr<graphics::IUVAnimation>> mUVAnimations;
		std::map<Path, std::shared_ptr<graphics::IMaterial>> mMaterials;

		std::atomic<size_t> mResourceCount;
		std::atomic<size_t> mLoadedResourceCount;
	};

	inline std::map<std::string, std::shared_ptr<graphics::IMaterial>> ResourceSystem::GetMaterials() const
	{
		return mMaterials;
	}
}