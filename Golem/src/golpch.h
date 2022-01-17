#pragma once

// Windows Spesific *******************************************************************************
#ifdef GOL_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
#endif

// Containers *************************************************************************************
#include <vector>
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

// Vulkan *****************************************************************************************
#include <vulkan/vulkan.h>

// Golem ******************************************************************************************
#include "Golem/Log.h"

