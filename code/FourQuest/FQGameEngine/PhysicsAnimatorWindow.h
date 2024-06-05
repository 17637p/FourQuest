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

	using AnimationClipContainer = std::vector<std::unordered_map<std::string, std::vector<DirectX::SimpleMath::Matrix>>>;

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
		void beginInputInt_AnimationSize();
		void beginInputText_AnimationName();
		void beginText_ObjectName();

		void beginTree_Animation(const int& number);
		void beginButton_AnimationPlay(const int& number);
		void beginButton_AnimationStop(const int& number);
		void beginButton_AnimationReset();
		void beginButton_AnimationSave(const int& number);

		/// <summary>
		/// 드래그 드랍 게임오브젝트 처리
		/// </summary>
		/// <param name="object"></param>
		void dragDropGameObject();

		void resetAnimation();

	private:
		GameProcess* mGameProcess;
		fq::game_module::Scene* mScene;

		bool mbIsOpen;
		bool mbIsStartScene;
		bool mbIsPlay;

		int mKeyFrameSize;
		int mAnimationSize;
		int mCurrentAnimaitionNumber;
		int mPlayAnimationClipNumber;
		int mCurrentKeyFrame;
		float mDurationTime;

		std::shared_ptr<fq::game_module::GameObject> mRegisteredObject;
		std::vector<std::string> mExtractObjectNames;
		std::vector<std::string> mAnimationClipNames;
		AnimationClipContainer mAnimationClipContainer;
		std::string mAnimationName;
		std::unordered_map<std::string, std::vector<DirectX::SimpleMath::Matrix>> mAnimationClip;

		fq::game_module::EventManager* mEventManager;
		fq::game_module::EventHandler	mStartSceneHandle;
	};
}