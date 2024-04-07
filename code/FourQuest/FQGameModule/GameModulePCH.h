#ifndef GAME_MODULE_PCH_H
#define GAME_MODULE_PCH_H

#define NOMINMAX
#include <windows.h>

/// XInput
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <string>
#include <functional>
#include <any>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <typeindex>
#include <utility>

#include <directxtk/SimpleMath.h>

#include "../FQReflect/FQReflect.h"

#include "GameModuleEnum.h"
#include "GameModuleType.h"

#endif //GAME_MODULE_PCH_H
