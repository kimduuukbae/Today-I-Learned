#pragma once

#include <iostream>
#include <list>
#include <tuple>
#include <string_view>
#include <string>
#include <vector>
#include <array>
#include <codecvt>
#include <concurrent_priority_queue.h>
#include <iterator>
#include <fstream>
#include <thread>
#include <queue>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_set>

#include <set>
#include "protocol.h"

extern std::array<std::array<int, 800>, 800> mapTile;