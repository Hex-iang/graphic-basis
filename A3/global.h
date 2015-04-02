#pragma once
#include "stdint.h"

#define WIN_WIDTH 		    512
#define WIN_HEIGHT 		    512
#define IMAGE_WIDTH  		  5.0
#define DIFFUSE_RAY_NUM   5

// numerical convienent macro
#define EPSILON           1e-8

#ifdef __linux__
#elif defined __APPLE__
#else
  #define INFINITY		1e8
#endif

class Statistic
{
public:
	static uint64_t intersect_cnt;
	static uint64_t intersect_test_cnt;
	static uint64_t ray_num_cnt;
};
