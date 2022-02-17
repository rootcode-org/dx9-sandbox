/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

#include <cstdlib>
#include <array>
#include <bitset>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <chrono>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <locale>
#include <codecvt>
#include <string>
#include <regex>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <exception>
#include <limits>
#include <new>
#include <typeinfo>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <complex>
#include <random>
#include <valarray>
#include <numeric>

// Windows platform
#if defined(_WINDOWS)
#include <windows.h>
#endif

#include "core/new.h"
#include "core/heap.h"
#include "core/keyboard.h"
#include "core/list.h"
#include "core/map.h"
#include "core/mouse.h"
#include "core/stack.h"
#include "core/tree.h"
#include "core/window.h"

#include "entity/entity.h"
#include "entity/entity_manager.h"

#include "graphics/viewport.h"
#include "graphics/vertex.h"
#include "graphics/d3d9.h"

#include "math/algebra.h"
#include "math/bezier.h"
#include "math/matrix.h"
#include "math/plane.h"
#include "math/polar.h"
#include "math/quaternion.h"
#include "math/vector.h"
