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
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include "Rynex/Core/Base.h"
#include "Rynex/Core/Log.h"
#include "Rynex/Core/Instrumentor.h"

#ifdef  RY_PLATFORM_WINDOWS
#include <Windows.h>
#endif //  RY_PLATFORM_WINDOWS