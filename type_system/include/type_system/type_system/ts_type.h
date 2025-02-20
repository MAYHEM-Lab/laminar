//
// Created by Lukas Brand on 09.01.2023.
//
#ifndef TYPE_SYSTEM_TS_TYPE_H
#define TYPE_SYSTEM_TS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ts_types.h"

/**
 * Function to load all values used into memory using the unloaded value from the MAIN WooF.
 * @param unloaded_value The unloaded value with NULL references.
 * @return The loaded value or NULL if something went wrong.
 */
struct ts_value* load_value(const struct ts_value* unloaded_value);

bool load_string_value(struct ts_value_string* string, const char* uri);
bool load_array_value(struct ts_value_array* array, const char* uri);

/**
 * Function to store value. Note the type system will modify information inside the storage system of the value.
 * These need to be written to the MAIN WooF otherwise information is lost.
 * @param value Value to store with potential modified storage system information after write.
 * @return true for success, false otherwise.
 */
bool write_value(struct ts_value* value);

#ifdef __cplusplus
}
#endif

#endif // TYPE_SYSTEM_TS_TYPE_H
