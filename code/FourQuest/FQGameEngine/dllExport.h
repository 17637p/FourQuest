
#include "FQGameEnginePCH.h"

#ifdef FQ_GAME_ENGINE_DLL_EXPORTS
#define FQ_ENGNIE_API __declspec(dllexport)
#else
#define FQ_ENGNIE_API __declspec(dllimport)
#endif

#ifdef FQ_GAME_ENGINE_DLL_EXPORTS
#define FQ_ENGNIE_TEMPLATE_API __declspec(dllexport)
#else
#define FQ_ENGNIE_TEMPLATE_API __declspec(dllimport)
#endif
