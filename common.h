#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define print(format, ...) printf(format "\n", ##__VA_ARGS__);