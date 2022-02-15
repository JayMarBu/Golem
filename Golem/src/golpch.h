#pragma once

// Windows Spesific *******************************************************************************
#ifdef GOL_PLATFORM_WINDOWS
	#include <windows.h>
#endif

// Containers *************************************************************************************
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
using std::string;
#include <set>
#include <unordered_set>
#include <array>
#include <queue>

// Utilities **************************************************************************************
#include <cassert>
#include <iostream>
#include <type_traits>
#include <cstdlib>
#include <stdexcept>
#include <fstream>
#include <limits>
#include <memory>
#include <cstring>
#include <numeric>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <random>
#include <sstream>
#include <functional>
#include <cstddef>
#include <algorithm>
#include <initializer_list>
#include <cstdarg>

// Vulkan *****************************************************************************************
#include <vulkan/vulkan.h>

// Golem ******************************************************************************************
#include "Golem/Log.h"
#include "Golem/Profiling/Instrumentor.h"

// glm ********************************************************************************************
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

