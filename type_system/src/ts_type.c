//
// Created by Lukas Brand on 24.01.2023.
//
#ifdef ESP8266
#include "ts_type.h"
#include "ts_settings.h"
#include "woofc.h"
#else
#include "type_system/ts_type.h"
#include "type_system/ts_settings.h"
#include <uuid/uuid.h>
#include <woofc.h>
#endif

#include <stdlib.h>
#include <string.h>

#define UUID_STR_LEN 37

#define STRING_NULL_CHARACTER_SIZE 1

#ifndef ESP8266
struct ts_string_partial {
    char partial_string[TS_STRING_SEGMENT_SIZE];
    unsigned long next_partial_sequence_id;
};
#endif

bool woof_exists(const char* woof_id) {
    if (WooFInvalid(WooFGetLatestSeqno(woof_id))) {
        return false;
    } else {
        return true;
    }
}

void unsigned_long_to_byte_array(unsigned long value, uint8_t* array) {
    int i;
    for (i = 0; i < 8; i++) {
        array[i] = (value >> (i * 8)) & 0xFF;
    }
}

unsigned long byte_array_to_unsigned_long(const uint8_t* const array) {
    unsigned long value = 0;
    int i;
    for (i = 0; i < 8; i++) {
        value |= (unsigned long)array[i] << (i * 8);
    }
    return value;
}



struct ts_value* load_value(const struct ts_value* const unloaded_value) {
    struct ts_value* return_value = malloc(sizeof(struct ts_value));

    return_value->type = unloaded_value->type;
    switch (unloaded_value->type) {
        case TS_UNINITIALIZED:
        case TS_UNKNOWN: {
            free(return_value);
            return NULL;
        }
        case TS_ERROR:
        case TS_EXCEPTION:
        case TS_BOOLEAN:
        case TS_BYTE:
        case TS_SHORT:
        case TS_INTEGER:
        case TS_LONG:
        case TS_UNSIGNED_BYTE:
        case TS_UNSIGNED_SHORT:
        case TS_UNSIGNED_INTEGER:
        case TS_UNSIGNED_LONG:
        case TS_FLOAT:
        case TS_DOUBLE:
        case TS_TIMESTAMP: 
	case TS_PRIM_STRING: {
            if (!value_deep_set(unloaded_value, return_value)) {
                free(return_value);
                return NULL;
            }
        } break;
        case TS_PRIM_LARGE_STRING: {
            if (!value_deep_set(unloaded_value, return_value)) {
                free(return_value);
                return NULL;
            }
        } break;
        case TS_PRIM_2D_DOUBLE_ARRAY: {
            for (size_t i = 0; i < TS_PRIM_2D_DOUBLE_ARRAY_ROWS; i++) {
                for (size_t j = 0; j < TS_PRIM_2D_DOUBLE_ARRAY_COLS; j++) {
                    return_value->value.ts_prim_2d_double_array[i][j] = unloaded_value->value.ts_prim_2d_double_array[i][j];
                }
            }
        } break;
        case TS_PRIM_DOUBLE_ARRAY: {
            if (!value_deep_set(unloaded_value, return_value)) {
                free(return_value);
                return NULL;
            }
        } break;
#ifndef ESP8266
        case TS_STRING: {
            // assume always local
            return_value->value.ts_string = unloaded_value->value.ts_string;
            if (!load_string_value(&return_value->value.ts_string, NULL)) {
                free(return_value);
                return NULL;
            }
        } break;
        case TS_ARRAY: {
            // assume always local
            return_value->value.ts_array = unloaded_value->value.ts_array;
            if (!load_array_value(&return_value->value.ts_array, NULL)) {
                free(return_value);
                return NULL;
            }
        } break;
#endif
        default: {
            free(return_value);
            return NULL;
        }
    }
    return return_value;
}

#ifndef ESP8266
bool load_string_value(struct ts_value_string* const string, const char* uri) {
    char woof_id[100];
    strcpy(woof_id, TS_STORAGE_PREFIX);
    strcat(woof_id, "-string");

    if (!woof_exists(woof_id)) {
        string->value = NULL;
        return false;
    }

    char* const complete_string = malloc(string->storage_system.element_size);
    strcpy(complete_string, "\0");
    unsigned long partial_string_sequence_id = byte_array_to_unsigned_long(string->storage_system.id);

    while (partial_string_sequence_id != 0) {
        struct ts_string_partial partial_string;
        const int get_result = WooFGet(woof_id, &partial_string, partial_string_sequence_id);
        if (!get_result) {
            free(complete_string);
            string->value = NULL;
            return false;
        }

        strncat(complete_string, partial_string.partial_string, TS_STRING_SEGMENT_SIZE);
        partial_string_sequence_id = partial_string.next_partial_sequence_id;
    }
    string->value = complete_string;
    return true;
}

void* woof_get_array_value(const char* const woof_id, size_t value_size, const size_t sequence_number) {
    void* array_value = malloc(value_size);
    const int is_get = WooFGet(woof_id, array_value, sequence_number);
    if (is_get < 0) {
        free(array_value);
        return NULL;
    }
    return array_value;
}

bool load_array_value(struct ts_value_array* const array, const char* uri) { // NOLINT(misc-no-recursion)
    char woof_id[200];
    // STRATEGY 
    if (uri){
        strcpy(woof_id, uri);
        strcat(woof_id, TS_STORAGE_PREFIX);
    }
    else{
        strcpy(woof_id, TS_STORAGE_PREFIX);
    }
    strcat(woof_id, "-array-");
    char uuid_string[UUID_STR_LEN];
    uuid_unparse_lower(array->storage_system.id, uuid_string);
    strcat(woof_id, uuid_string);

    // Wait if WooF does not exist or no entry is written
    while (!woof_exists(woof_id)) {}
    
    while (WooFGetLatestSeqno(woof_id) == 0) {}
    
    const unsigned long index = WooFGetLatestSeqno(woof_id);

    switch (array->type) {
        case TS_UNINITIALIZED:
            array->value = NULL;
        case TS_BOOLEAN:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(bool), index);
            break;
        case TS_BYTE:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(int8_t), index);
            break;
        case TS_SHORT:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(int16_t), index);
            break;
        case TS_INTEGER:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(int32_t), index);
            break;
        case TS_TIMESTAMP:
        case TS_LONG:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(int64_t), index);
            break;
        case TS_UNSIGNED_BYTE:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(uint8_t), index);
            break;
        case TS_UNSIGNED_SHORT:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(uint16_t), index);
            break;
        case TS_UNSIGNED_INTEGER:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(uint32_t), index);
            break;
        case TS_UNSIGNED_LONG:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(uint64_t), index);
            break;
        case TS_FLOAT:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(float), index);
            break;
        case TS_DOUBLE:
            array->value = woof_get_array_value(woof_id, array->size * sizeof(double), index);
            break;
        case TS_STRING: {
            array->value = woof_get_array_value(woof_id, array->size * sizeof(struct ts_value_string), index);
            if (array->value != NULL) {
                for (size_t i = 0; i < array->size; i++) {
                    struct ts_value_string* const array_element = (struct ts_value_string*)array->value;
                    const bool is_element_loaded = load_string_value(&array_element[i], uri);
                    if (!is_element_loaded) {
                        // delete all previous allocated elements
                        for (size_t j = 0; j < i; j++) {
                            value_deep_delete(&array->value[j]);
                        }
                        array->value = NULL;
                    }
                }
            }
        } break;
        case TS_ARRAY: {
            array->value = woof_get_array_value(woof_id, array->size * sizeof(struct ts_value_array), index);
            if (array->value != NULL) {
                for (size_t i = 0; i < array->size; i++) {
                    struct ts_value_array* const array_element = (struct ts_value_array*)array->value;
                    const bool is_element_loaded = load_array_value(&array_element[i], uri);
                    if (!is_element_loaded) {
                        // delete all previous allocated elements
                        for (size_t j = 0; j < i; j++) {
                            value_deep_delete(&array->value[j]);
                        }
                        array->value = NULL;
                    }
                }
            }
        } break;
        default:
            array->value = NULL;
    }
    return (array->value != NULL) ? true : false;
}


bool write_string_value(struct ts_value_string* string);
bool write_array_value(struct ts_value_array* array);
#endif

bool write_value(struct ts_value* const value) {
    switch (value->type) {
        case TS_UNINITIALIZED:
            return false;
        case TS_UNKNOWN:
        case TS_ERROR:
        case TS_EXCEPTION:
        case TS_BOOLEAN:
        case TS_BYTE:
        case TS_SHORT:
        case TS_INTEGER:
        case TS_LONG:
        case TS_UNSIGNED_BYTE:
        case TS_UNSIGNED_SHORT:
        case TS_UNSIGNED_INTEGER:
        case TS_UNSIGNED_LONG:
        case TS_FLOAT:
        case TS_DOUBLE:
        case TS_TIMESTAMP:
            // primitives do not need separate WooFs
            return true;
#ifndef ESP8266
        case TS_STRING:
            return write_string_value(&value->value.ts_string);
        case TS_ARRAY:
            return write_array_value(&value->value.ts_array);
#endif
        default:
            return false;
    }
}

#ifndef ESP8266
bool write_string_value(struct ts_value_string* const string) {
    char woof_id[100];
    strcpy(woof_id, TS_STORAGE_PREFIX);
    strcat(woof_id, "-string");

    if (!woof_exists(woof_id)) {
        const int create_result = WooFCreate(woof_id, sizeof(struct ts_string_partial), TS_STRING_STORAGE_SIZE);
        if (!create_result) {
            return false;
        }
    }

    const size_t string_complete_length = strlen(string->value) + STRING_NULL_CHARACTER_SIZE;
    const size_t segment_count =
        (string_complete_length / TS_STRING_SEGMENT_SIZE) + (string_complete_length % TS_STRING_SEGMENT_SIZE != 0);

    unsigned long partial_string_sequence_id = 0;
    for (unsigned long segment_index = segment_count; segment_index > 0; segment_index--) {
        const unsigned long string_index = TS_STRING_SEGMENT_SIZE * (segment_index - 1);

        struct ts_string_partial partial_string;
        partial_string.next_partial_sequence_id = partial_string_sequence_id;
        const char* partial_string_start = string->value + string_index;
        strncpy(partial_string.partial_string, partial_string_start, TS_STRING_SEGMENT_SIZE);

        partial_string_sequence_id = WooFPut(woof_id, NULL, &partial_string);
        if (WooFInvalid(partial_string_sequence_id)) {
            return false;
        }
    }
    unsigned_long_to_byte_array(partial_string_sequence_id, string->storage_system.id);
    string->storage_system.element_size = string_complete_length;
    // Invalidate written array address to avoid writes of wrong memory locations
    string->value = NULL;
    return true;
}

bool write_array_value(struct ts_value_array* const array) { // NOLINT(misc-no-recursion)
    char woof_id[100];
    strcpy(woof_id, TS_STORAGE_PREFIX);
    strcat(woof_id, "-array-");
    char uuid_string[UUID_STR_LEN];
    uuid_unparse_lower(array->storage_system.id, uuid_string);
    strcat(woof_id, uuid_string);

    size_t woof_element_size;
    switch (array->type) {
        case TS_UNINITIALIZED:
        case TS_ERROR:
        case TS_EXCEPTION:
            return false;
        case TS_UNKNOWN:
            return true;
        case TS_BOOLEAN: {
            woof_element_size = array->size * sizeof(bool);
        } break;
        case TS_BYTE: {
            woof_element_size = array->size * sizeof(int8_t);
        } break;
        case TS_SHORT: {
            woof_element_size = array->size * sizeof(int16_t);
        } break;
        case TS_INTEGER: {
            woof_element_size = array->size * sizeof(int32_t);
        } break;
        case TS_TIMESTAMP:
        case TS_LONG: {
            woof_element_size = array->size * sizeof(int64_t);
        } break;
        case TS_UNSIGNED_BYTE: {
            woof_element_size = array->size * sizeof(uint8_t);
        } break;
        case TS_UNSIGNED_SHORT: {
            woof_element_size = array->size * sizeof(uint16_t);
        } break;
        case TS_UNSIGNED_INTEGER: {
            woof_element_size = array->size * sizeof(uint32_t);
        } break;
        case TS_UNSIGNED_LONG: {
            woof_element_size = array->size * sizeof(uint64_t);
        } break;
        case TS_FLOAT: {
            woof_element_size = array->size * sizeof(float);
        } break;
        case TS_DOUBLE: {
            woof_element_size = array->size * sizeof(double);
        } break;
        case TS_PRIM_STRING: {
	    return false; // TODO: allow arrays of prim strings
	} break;
        case TS_PRIM_LARGE_STRING: {
            return false; // TODO: allow arrays of prim large strings
        } break;
        case TS_PRIM_2D_DOUBLE_ARRAY: {
            return false; // TODO: allow arrays of prim 2d arrays
        } break;
        case TS_PRIM_DOUBLE_ARRAY: {
            return false; // TODO: allow arrays of prim 1d arrays
        } break;
        case TS_STRING: {
            woof_element_size = array->size * sizeof(struct ts_value_string);
            for (size_t i = 0; i < array->size; i++) {
                struct ts_value_string* const string_element = (struct ts_value_string*)array->value;
                const bool string_write_result = write_string_value(&string_element[i]);
                if (!string_write_result) {
                    return false;
                }
            }
        } break;
        case TS_ARRAY: {
            woof_element_size = array->size * sizeof(struct ts_value_array);
            for (size_t i = 0; i < array->size; i++) {
                struct ts_value_array* const array_element = (struct ts_value_array*)array->value;
                const bool array_write_result = write_array_value(&array_element[i]);
                if (!array_write_result) {
                    return false;
                }
            }
        } break;
        default: {
            return false;
        }
    }
    const int create_success = WooFCreate(woof_id, woof_element_size, TS_ARRAY_HISTORY_SIZE);
    if (create_success < 0) {
        return false;
    }
    const unsigned long put_success = WooFPut(woof_id, NULL, array->value);
    if (WooFInvalid(put_success)) {
        return false;
    }
    // Invalidate written array address to avoid writes of wrong memory locations
    array->value = NULL;
    return true;
}
#endif
