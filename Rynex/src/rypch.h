#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>

#include <array>
#include <chrono>
#include <filesystem>
#include <future>
#include <optional>
#include <queue>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <Rynex/Core/Base.h>
#include <Rynex/Core/Instrumentor.h>
#include <Rynex/Core/Log.h>
#include <Rynex/Project/Path.h>


#ifdef  RY_PLATFORM_WINDOWS
#include <Windows.h>
#endif //  RY_PLATFORM_WINDOWS