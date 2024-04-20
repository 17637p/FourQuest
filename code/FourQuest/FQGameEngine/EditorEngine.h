#pragma once

#include <memory>

#include "IEngine.h"

namespace fq
{
	namespace game_engine
	{
		class GameProcess;
		class EditorProcess;
		class TmpD3D;

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

		private:
			std::unique_ptr<GameProcess> mGame;
			std::unique_ptr<EditorProcess> mEditor;
			std::unique_ptr<TmpD3D> mD3D;
		};
	}
}

