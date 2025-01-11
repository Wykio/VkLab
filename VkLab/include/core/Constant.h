#ifndef CONSTANT_H
#define CONSTANT_H

#include <cstdint>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2; // 2 because we don’t want the CPU to get too far ahead of the GPU.

#endif // CONSTANT_H