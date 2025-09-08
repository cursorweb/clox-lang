#pragma once

#include "common.h"

typedef enum ValueType
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER
} ValueType;

typedef struct Value
{
    ValueType type;

    // no union name here, because we don't want a global `union ValueUnion`
    // Usage: `value.as.boolean`
    union
    {
        bool boolean;
        double number;
    } as;

} Value;

// create a Value struct for bool
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NUM_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
// todo: why need initialize number?
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})

#define AS_BOOL(value) (value).as.boolean
#define AS_NUM(value) (value).as.number

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NUM(value) ((value).type == VAL_NUMBER)
#define IS_NIL(value) ((value).type == VAL_NIL)

// constant pool?
typedef struct ValueArray
{
    int count;
    int capacity;

    // ~vector
    Value* values;
} ValueArray;

void init_value_array(ValueArray* array);
void write_value_array(ValueArray* array, Value value);
void free_value_array(ValueArray* array);

// Used to print values, not for debugging (but can also use to debug)!
// don't forget to print "\n" after!
void print_value(Value value);
bool values_equal(Value a, Value b);