#pragma once

#include "vm.h"

// returns TRUE if compiler had an error
bool compile(const char* source, Chunk* chunk);