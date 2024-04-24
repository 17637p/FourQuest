#pragma once

#include <memory>

#include "IEngine.h"

namespace fq
{
	namespace game_engine
	{
		class GameProcess;
		class EditorProcess;

		class EditorEngine : public IEngine
		{
		public:
			EditorEngine();
			~EditorEngine();

			/// <summary>
			/// �����͸� �ʱ�ȭ�մϴ� 
			/// </summary>
			void Initialize() override;
			
			/// <summary>
			/// �����͸� �����մϴ�
			/// </summary>
			void Process() override;
			
			/// <summary>
			/// �����͸� �����մϴ� 
			/// </summary>
			void Finalize() override;

			/// <summary>
			/// ������ â�� �������մϴ�
			/// </summary>
			void RenderEditorWinodw();

			/// <summary>
			/// ������ ���� �ʱ�ȭ�� �մϴ� 
			/// </summary>
			void InitializeEditor();

			/// <summary>
			/// ������ ���� ������Ʈ�� �����մϴ�
			/// </summary>
			void UpdateEditor(float dt);

		private:
			std::unique_ptr<GameProcess> mGameProcess;
			std::unique_ptr<EditorProcess> mEditor;
		};
	}
}

