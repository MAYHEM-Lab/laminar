//
// Created by Lukas Brand on 25.04.2023.
//
#include "type_system/types/ts_array.h"

#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

void set_boolean_array(struct ts_value* value, const bool array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    bool* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(bool) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_BOOLEAN,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_byte_array(struct ts_value* value, const int8_t array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    int8_t* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(int8_t) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_BYTE,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_short_array(struct ts_value* value, const int16_t array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    int16_t* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(int16_t) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_SHORT,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_integer_array(struct ts_value* value, const int32_t array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    int32_t* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(int32_t) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_INTEGER,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_long_array(struct ts_value* value, const int64_t array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    int64_t* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(int64_t) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_LONG,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_unsigned_byte_array(struct ts_value* value, const uint8_t array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    uint8_t* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(uint8_t) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_UNSIGNED_BYTE,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_unsigned_short_array(struct ts_value* value, const uint16_t array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    uint16_t* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(uint16_t) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_UNSIGNED_SHORT,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_unsigned_integer_array(struct ts_value* value, const uint32_t array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    uint32_t* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(uint32_t) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_UNSIGNED_INTEGER,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_unsigned_long_array(struct ts_value* value, const uint64_t array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    uint64_t* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(uint64_t) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_UNSIGNED_LONG,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_float_array(struct ts_value* value, const float array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    float* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(float) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_FLOAT,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_double_array(struct ts_value* value, const double array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    double* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(double) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_DOUBLE,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void set_timestamp_array(struct ts_value* value, const int64_t array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    int64_t* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(int64_t) * size);
        for (size_t i = 0; i < size; i++) {
            copy_array[i] = array[i];
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_TIMESTAMP,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}


void get_boolean_array(bool array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        *(array + array_index) = *((bool*)array_value.value + array_index);
    }
}

void get_byte_array(int8_t array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        *(array + array_index) = *((int8_t*)array_value.value + array_index);
    }
}

void get_short_array(int16_t array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((int16_t*)array_value.value + array_index);
    }
}

void get_integer_array(int32_t array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((int32_t*)array_value.value + array_index);
    }
}

void get_long_array(int64_t array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((int64_t*)array_value.value + array_index);
    }
}

void get_unsigned_byte_array(uint8_t array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((uint8_t*)array_value.value + array_index);
    }
}

void get_unsigned_short_array(uint16_t array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((uint16_t*)array_value.value + array_index);
    }
}

void get_unsigned_integer_array(uint32_t array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((uint32_t*)array_value.value + array_index);
    }
}

void get_unsigned_long_array(uint64_t array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((uint64_t*)array_value.value + array_index);
    }
}

void get_float_array(float array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((float*)array_value.value + array_index);
    }
}

void get_double_array(double array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((double*)array_value.value + array_index);
    }
}

void get_timestamp_array(int64_t array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        array[array_index] = *((int64_t*)array_value.value + array_index);
    }
}


struct ts_value* value_from_boolean_array(const bool array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_boolean_array(value, array, size);
    return value;
}

struct ts_value* value_from_byte_array(const int8_t array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_byte_array(value, array, size);
    return value;
}

struct ts_value* value_from_short_array(const int16_t array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_short_array(value, array, size);
    return value;
}

struct ts_value* value_from_integer_array(const int32_t array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_integer_array(value, array, size);
    return value;
}

struct ts_value* value_from_long_array(const int64_t array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_long_array(value, array, size);
    return value;
}

struct ts_value* value_from_unsigned_byte_array(const uint8_t array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_unsigned_byte_array(value, array, size);
    return value;
}

struct ts_value* value_from_unsigned_short_array(const uint16_t array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_unsigned_short_array(value, array, size);
    return value;
}

struct ts_value* value_from_unsigned_integer_array(const uint32_t array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_unsigned_integer_array(value, array, size);
    return value;
}

struct ts_value* value_from_unsigned_long_array(const uint64_t array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_unsigned_long_array(value, array, size);
    return value;
}

struct ts_value* value_from_float_array(const float array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_float_array(value, array, size);
    return value;
}

struct ts_value* value_from_double_array(const double array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_double_array(value, array, size);
    return value;
}

struct ts_value* value_from_timestamp_array(const int64_t array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_long_array(value, array, size);
    return value;
}


bool* value_to_boolean_array(const struct ts_value* const value) {
    bool* return_array = malloc(value->value.ts_array.size * sizeof(bool));
    get_boolean_array(return_array, value);
    return return_array;
}

int8_t* value_to_byte_array(const struct ts_value* const value) {
    int8_t* return_array = malloc(value->value.ts_array.size * sizeof(int8_t));
    get_byte_array(return_array, value);
    return return_array;
}

int16_t* value_to_short_array(const struct ts_value* const value) {
    int16_t* return_array = malloc(value->value.ts_array.size * sizeof(int16_t));
    get_short_array(return_array, value);
    return return_array;
}

int32_t* value_to_integer_array(const struct ts_value* const value) {
    int32_t* return_array = malloc(value->value.ts_array.size * sizeof(int32_t));
    get_integer_array(return_array, value);
    return return_array;
}

int64_t* value_to_long_array(const struct ts_value* const value) {
    int64_t* return_array = malloc(value->value.ts_array.size * sizeof(int64_t));
    get_long_array(return_array, value);
    return return_array;
}

uint8_t* value_to_unsigned_byte_array(const struct ts_value* const value) {
    uint8_t* return_array = malloc(value->value.ts_array.size * sizeof(uint8_t));
    get_unsigned_byte_array(return_array, value);
    return return_array;
}

uint16_t* value_to_unsigned_short_array(const struct ts_value* const value) {
    uint16_t* return_array = malloc(value->value.ts_array.size * sizeof(uint16_t));
    get_unsigned_short_array(return_array, value);
    return return_array;
}

uint32_t* value_to_unsigned_integer_array(const struct ts_value* const value) {
    uint32_t* return_array = malloc(value->value.ts_array.size * sizeof(uint32_t));
    get_unsigned_integer_array(return_array, value);
    return return_array;
}

uint64_t* value_to_unsigned_long_array(const struct ts_value* const value) {
    uint64_t* return_array = malloc(value->value.ts_array.size * sizeof(uint64_t));
    get_unsigned_long_array(return_array, value);
    return return_array;
}

float* value_to_float_array(const struct ts_value* const value) {
    float* return_array = malloc(value->value.ts_array.size * sizeof(float));
    get_float_array(return_array, value);
    return return_array;
}

double* value_to_double_array(const struct ts_value* const value) {
    double* return_array = malloc(value->value.ts_array.size * sizeof(double));
    get_double_array(return_array, value);
    return return_array;
}

int64_t* value_to_timestamp_array(const struct ts_value* const value) {
    int64_t* return_array = malloc(value->value.ts_array.size * sizeof(int64_t));
    get_long_array(return_array, value);
    return return_array;
}


#ifndef PLACEHOLDER // TS_STRING_INCLUDE
void set_string_array(struct ts_value* const value, const char* array[], const size_t size) {
    struct ts_storage_system storage_system;
    uuid_generate(storage_system.id);

    size_t new_size;
    struct ts_value_string* copy_array;
    if (size == 0 || array == NULL) {
        new_size = 0;
        copy_array = NULL;
    } else {
        new_size = size;
        copy_array = malloc(sizeof(struct ts_value_string) * size);
        for (size_t i = 0; i < size; i++) {
            const char* const string = array[i];
            const size_t string_length = strlen(string);
            char* const copied_string = malloc(string_length + 1);
            strcpy(copied_string, string);
            copy_array[i].value = copied_string;
            copy_array[i].storage_system.element_size = string_length + 1;
        }
    }

    struct ts_value_array value_array = {
        .storage_system = storage_system,
        .type = TS_STRING,
        .size = new_size,
        .value = copy_array,
    };
    value->value.ts_array = value_array;
    value->type = TS_ARRAY;
}

void get_string_array(char* array[], const struct ts_value* const value) {
    struct ts_value_array array_value = value->value.ts_array;
    if (array_value.value == NULL) {
        array = NULL;
        return;
    }
    for (int array_index = 0; array_index < array_value.size; array_index++) {
        const struct ts_value_string* const string = (struct ts_value_string*)array_value.value + array_index;
        char* const copied_string = malloc(string->storage_system.element_size);
        strcpy(copied_string, string->value);
        array[array_index] = copied_string;
    }
}

struct ts_value* value_from_string_array(const char* array[], const size_t size) {
    struct ts_value* value = malloc(sizeof(struct ts_value));
    set_string_array(value, array, size);
    return value;
}

char** value_to_string_array(const struct ts_value* const value) {
    char** return_array = malloc(value->value.ts_array.size * sizeof(char *));
    get_string_array(return_array, value);
    return return_array;
}
#endif


size_t get_array_size(const struct ts_value* const value) {
    return value->value.ts_array.size;
}

enum ts_types get_array_type(const struct ts_value* const value) {
    return value->value.ts_array.type;
}