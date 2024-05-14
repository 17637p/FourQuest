#pragma once
#pragma once
#include "IEditorWindow.h"

#include <vector>

#include "../FQGameModule/GameModule.h"
#include "../FQCommon/FQCommonLoader.h"

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	using AnimationClipContainer = std::vector<std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Matrix>>>;

	class PhysicsAnimatorWindow : public IEditorWindow
	{
	public:
		PhysicsAnimatorWindow();
		~PhysicsAnimatorWindow();

		void Initialize(GameProcess* game);

		void Render() override;
		void UpdateAnimation(float dt);

		bool& IsWindowOpen() { return mbIsOpen; }
		void StartScene();

	private:
		void beginAnimatiorEditor();

		void beginInputInt_KeyFrameSize();
		void beginText_ObjectName();

		void beginTree_Animation(const int& number);
		void beginButton_AnimationPlay(const int& number);
		void beginButton_AnimationSave(const int& number);

		/// <summary>
		/// 드래그 드랍 게임오브젝트 처리
		/// </summary>
		/// <param name="object"></param>
		void dragDropGameObject();

	private:
		GameProcess* mGameProcess;
		fq::game_module::Scene* mScene;

		bool mbIsOpen;
		bool mbIsStartScene;
		bool mbIsPlay;

		int mKeyFrameSize;
		int mPlayAnimationClipNumber;
		float mDurationTime;

		fq::game_module::GameObject*	mRegisteredObject;
		std::vector<fq::game_module::GameObject*> mExtractObjects;
		AnimationClipContainer mAnimationClipContainer;
		std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Matrix>> mAnimationClip;

		fq::game_module::EventManager*	mEventManager;
		fq::game_module::EventHandler	mStartSceneHandle;
	};
}