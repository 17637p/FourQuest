#pragma once

#include <windows.h>

#include "dllExport.h"

namespace fq
{
	namespace game_engine
	{
		/// <summary>
		/// 엔진 인터페이스 
		/// </summary>
		class IEngine
		{
		public:
			virtual ~IEngine() {}

			virtual FQ_ENGNIE_API bool Initialize() abstract;

			virtual FQ_ENGNIE_API void Process() abstract;

			virtual FQ_ENGNIE_API void Finalize() abstract;
		};

		/// <summary>
		/// 게임 엔진 Exporter
		/// </summary>
		class Exporter
		{
		public:
			/// <summary>
			/// 게임엔진을 할당하고 반환
			/// </summary>
			/// <returns>할당한 게임엔진</returns>
			static FQ_ENGNIE_API IEngine* GetGameEngine();

			/// <summary>
			/// 에디터 엔진을 할당하고 반환
			/// </summary>
			/// <returns>할당한 툴엔진</returns>
			static FQ_ENGNIE_API IEngine* GetEditorEngine();

			/// <summary>
			/// 할당된 게임엔진을 삭제
			/// </summary>
			/// <returns></returns>
			static FQ_ENGNIE_API void DeleteEngine();
			 
		private:
			static IEngine* mEngine;
		};
	}
}
