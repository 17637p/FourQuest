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
			/// 에디터를 초기화합니다 
			/// </summary>
			void Initialize() override;
			
			/// <summary>
			/// 에디터를 실행합니다
			/// </summary>
			void Process() override;
			
			/// <summary>
			/// 에디터를 종료합니다 
			/// </summary>
			void Finalize() override;

			/// <summary>
			/// 에디터 창을 랜더링합니다
			/// </summary>
			void RenderEditorWinodw();

			/// <summary>
			/// 에디터 관련 초기화를 합니다 
			/// </summary>
			void InitializeEditor();

		private:
			std::unique_ptr<GameProcess> mGame;
			std::unique_ptr<EditorProcess> mEditor;
			std::unique_ptr<TmpD3D> mD3D;
		};
	}
}

