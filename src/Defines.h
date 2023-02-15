#pragma once

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)
#define Gigabytes(value) (Megabytes(value) * 1024LL)
#define Terabytes(value) (Gigabytes(value) * 1024LL)
#define Assert(x) { if (!(x)) __debugbreak(); }
#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#define GAME_API __declspec(dllexport)

#define PATH_MAX 260

#define DELTA_TIME_SCALE 1.0f //(1 / 10.0f)