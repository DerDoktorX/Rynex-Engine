#pragma once


#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <array>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <regex>
#include <optional>
#include <queue>

// fast hash map
#include <robin_hood/robin_hood.h>

#include <Rynex/Core/Base.h>
#include <Rynex/Core/Log.h>
#include <Rynex/Core/Instrumentor.h>
#include <Rynex/Project/Path.h>


#ifdef  RY_PLATFORM_WINDOWS
#include <Windows.h>
#endif //  RY_PLATFORM_WINDOWS