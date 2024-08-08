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

		const fq::common::Model& GetModel(const Path& path)const ;

		void LoadSceneResource(fq::event::PreOnLoadScene event);
		
		void LoadAnimator(fq::game_module::Animator* animator);
		void LoadModelResource(const Path& path);
		void LoadAnimation(const Path& path);
		void LoadMaterial(const Path& path);

	private:
		GameProcess* mGameProcess;
		EventHandler mPreOnLoadSceneHandler;
		fq::game_module::AnimatorControllerLoader mLoader;

		mutable Mutex mModelMutex;
		mutable	Mutex mStaticMeshMutex;
		mutable Mutex mSkinnedMeshMutex;
		mutable Mutex mNodeHierarchyMutex;
		mutable Mutex mAnimationMutex;
		 
		std::map<Path, fq::common::Model> mModels;
		std::map<Path, std::shared_ptr<graphics::IStaticMesh>> mStaticMeshes;
		std::map<Path, std::shared_ptr<graphics::ISkinnedMesh>> mSkinnedMeshes;
		std::map<Path, std::shared_ptr<graphics::INodeHierarchy>> mNodeHierarchies;
		std::map<Path, std::shared_ptr<graphics::IAnimation>> mAnimations;
		std::map<Path, std::shared_ptr<graphics::IUVAnimation>> mUVAnimations;
		std::map<Path, std::shared_ptr<graphics::IMaterial>> mMaterials;
		std::map<Path, std::shared_ptr<graphics::IParticleMaterial>> mParticleMaterials;
		std::map<Path, std::shared_ptr<graphics::IDecalMaterial>> mDecalMaterials;
	};
}