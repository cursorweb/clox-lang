#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define print(format, ...) printf(format "\n", ##__VA_ARGS__);

// disassemble instructions as they are made (compiler)
#define DEBUG_PRINT_CODE
// disassemble instructions as they execute (vm)
#define DEBUG_TRACE_EXECUTION