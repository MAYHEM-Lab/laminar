//
// Created by Lukas Brand on 29.03.23.
//

#ifndef TYPE_SYSTEM_TS_TYPES_H
#define TYPE_SYSTEM_TS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define NOPRIM

#ifdef NOPRIM
#define TS_PRIM_STRING_SIZE 16
#define TS_PRIM_L_STRING_SIZE 16
#define TS_PRIM_2D_DOUBLE_ARRAY_ROWS 1
#define TS_PRIM_2D_DOUBLE_ARRAY_COLS 1
#define TS_PRIM_DOUBLE_ARRAY_SIZE 1
#else
#define TS_PRIM_STRING_SIZE 1152
#define TS_PRIM_L_STRING_SIZE 71680
#define TS_PRIM_2D_DOUBLE_ARRAY_ROWS 11
#define TS_PRIM_2D_DOUBLE_ARRAY_COLS 10
#define TS_PRIM_DOUBLE_ARRAY_SIZE 11
#endif

/**
 * The type system type definitions. All usable types are defined here.
 */
enum ts_types {
    // Special & Error Types
    TS_UNINITIALIZED = 0,
    TS_UNKNOWN = 1,
    TS_ERROR = 2,     /* TS_ERROR(x) -> use uint32_t as enum to give more detail about the error */
    TS_EXCEPTION = 3, /* TS_EXCEPTION(x) -> use uint32_t as enum to give more detail about the exception */
    // 4-9: Space for custom special types

    // Simple Types
    TS_BOOLEAN = 10,
    TS_BYTE = 20,
    TS_SHORT = 21,
    TS_INTEGER = 22,
    TS_LONG = 23,
    TS_UNSIGNED_BYTE = 30,
    TS_UNSIGNED_SHORT = 31,
    TS_UNSIGNED_INTEGER = 32,
    TS_UNSIGNED_LONG = 33,
    TS_FLOAT = 40,
    TS_DOUBLE = 41,
    TS_PRIM_STRING = 42, // to fix remote strings
    TS_PRIM_LARGE_STRING = 43,
    TS_PRIM_2D_DOUBLE_ARRAY = 44, // to fix remote 2d double arrays
    TS_PRIM_DOUBLE_ARRAY = 45, // to fix remote 1d double array
    TS_TIMESTAMP = 50,
    // 51-99: Space for custom primitive types

    // Composite Types
    TS_STRING = 100,
    TS_ARRAY = 101,
    TS_LIST = 102,
    TS_MAP = 103,
    TS_RECORD = 104,
    TS_QUEUE = 105,
    TS_BINARY = 106,
    TS_STREAM_ARRAY = 107,
    // ...: Space for custom composite types
};

/**
 * The storage system, responsible for information necessary to load composite types from additional WooFs
 */
struct ts_storage_system {
    uint8_t id[16];      // Unique identifier for composite items. Can but must not be used in combination with the
                         // uuid.h library.
    size_t element_size; // Absolute element size of composite item in bytes.
};


/* **************** STRING **************** */
struct ts_value_string {
    struct ts_storage_system storage_system;
    const char* value;
};

/* **************** ARRAY **************** */
struct ts_value_array {
    struct ts_storage_system storage_system;
    size_t size;
    enum ts_types type;
    void* value;
};

/* **************** LIST **************** */
struct ts_value_list {
    struct ts_value* value;
    struct ts_value_list* next;
};

/* **************** MAP **************** */
struct ts_value_map {
    struct ts_storage_system storage_system;
    enum ts_types key_type;
    void* key_value;

    enum ts_types value_type;
    void* value_value;
};

/* **************** QUEUE **************** */
struct ts_value_queue {
    struct ts_storage_system storage_system;
    size_t size;
    enum ts_types type;
    void* value;
};

/* **************** RECORD **************** */
struct ts_record_element {
    char identifier[21];
    struct ts_value* value;
    struct ts_record_element* next_element;
};

struct ts_value_record {
    struct ts_storage_system storage_system;
    struct ts_record_element element;
};


/* **************** VALUE **************** */
union ts_values {
    uint32_t ts_unsigned_int;
    uint64_t ts_unsigned_long;
    int32_t ts_int;
    int64_t ts_long;
    float ts_float;
    double ts_double;
    char ts_prim_string[TS_PRIM_STRING_SIZE];
    char ts_prim_large_string[TS_PRIM_L_STRING_SIZE];
    double ts_prim_2d_double_array[TS_PRIM_2D_DOUBLE_ARRAY_ROWS][TS_PRIM_2D_DOUBLE_ARRAY_COLS];
    double ts_prim_double_array[TS_PRIM_DOUBLE_ARRAY_SIZE];
    struct ts_value_string ts_string;
    struct ts_value_array ts_array;
    struct ts_value_list ts_list;
    struct ts_value_map ts_map;
    struct ts_value_queue ts_queue;
    struct ts_value_record ts_record;
};

struct ts_value {
    enum ts_types type;
    union ts_values value;
#ifdef ESP8266
    uint64_t pad;
#endif
};

#define TS_VALUE_SIZE sizeof(struct ts_value)

char* value_type_as_string(enum ts_types type);
char* value_as_string(const struct ts_value* value);


bool value_deep_set(const struct ts_value* source, struct ts_value* destination);
struct ts_value* value_deep_copy(const struct ts_value* source);
bool value_deep_delete(struct ts_value* value);

#ifdef __cplusplus
}
#endif

#endif // TYPE_SYSTEM_TS_TYPES_H
