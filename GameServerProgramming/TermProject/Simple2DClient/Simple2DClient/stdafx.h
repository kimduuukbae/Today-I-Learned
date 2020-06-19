#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <windows.h>
#include <iostream>
#include <unordered_map>
#include <chrono>

#include <deque>

#include <string>
#include <codecvt>
#include "..\..\IOCPGameServer\IOCPGameServer\protocol.h"

constexpr auto SCREEN_WIDTH = 20;
constexpr auto SCREEN_HEIGHT = 20;

constexpr auto TILE_WIDTH = 65;
constexpr auto WINDOW_WIDTH = TILE_WIDTH * SCREEN_WIDTH / 2 + 10;   // size of window
constexpr auto WINDOW_HEIGHT = TILE_WIDTH * SCREEN_WIDTH / 2 + 10;
constexpr auto BUF_SIZE = 200;
constexpr auto MAX_USER = 10;

std::wstring utf8_to_wstring(const std::string_view& str);
