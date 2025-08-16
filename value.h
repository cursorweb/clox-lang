#pragma once

#include "common.h"

typedef double Value;

// constant pool?
typedef struct ValueArray
{
    int count;
    int capacity;
    Value* values;
} ValueArray;

void init_value_array(ValueArray* array);
void write_value_array(ValueArray* array, Value value);
void free_value_array(ValueArray* array);

// TODO: why here and not in debug.c?
void print_value(Value value);