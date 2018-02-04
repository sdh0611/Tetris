#pragma once

#include "targetver.h"
#include <deque>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

#define KEY_DOWN(vk_code) ( (GetAsyncKeyState(vk_code) & 0x8000)?1:0)
#define KEY_UP(vk_code) ( (GetAsyncKeyState(vk_code) & 0x8000)?0:1)

#define WIDTH 10
#define HEIGHT 20

struct Point {
	int x;
	int y;
	bool operator=(const Point& other);
};

enum { EMPTY, WALL, BLOCK, INACTIVE };


void GotoXy(int x, int y);
