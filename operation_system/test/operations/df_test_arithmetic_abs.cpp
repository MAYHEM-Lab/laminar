//
// Created by Lukas Brand on 01.04.23.
//
#include "../../src/operations/df_arithmetic.h"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("arithmetic: abs: no value") {
    const enum df_arithmetic_ops arithmetic_operation = DF_ARITH_ABS;
    const struct ts_value* op_list[0] = {};
    const int operand_count = 0;
    struct ts_value* result;
    int return_value;
    SECTION("TS_UNINITIALIZED") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNINITIALIZED, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_BYTE") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_BYTE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_SHORT") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_SHORT, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_INTEGER") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_INTEGER, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_LONG") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_LONG, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_UNSIGNED_BYTE") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNSIGNED_BYTE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_UNSIGNED_SHORT") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNSIGNED_SHORT, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_UNSIGNED_INTEGER") {
        return_value = df_arithmetic_operation_with_type(
            arithmetic_operation, op_list, operand_count, TS_UNSIGNED_INTEGER, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_UNSIGNED_LONG") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNSIGNED_LONG, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_DOUBLE") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_DOUBLE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_DATETIME") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_TIMESTAMP, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_STRING") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_STRING, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_ARRAY") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_ARRAY, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_LIST") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_LIST, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_QUEUE") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_QUEUE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_RECORD") {
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_RECORD, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
}

TEST_CASE("arithmetic: abs: one value") {
    const enum df_arithmetic_ops arithmetic_operation = DF_ARITH_ABS;
    const int operand_count = 1;
    struct ts_value* result;
    int return_value;
    SECTION("TS_UNINITIALIZED") {
        const struct ts_value op1 = {.type = TS_UNINITIALIZED, .value = {}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNINITIALIZED, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_BYTE") {
        const struct ts_value op1 = {.type = TS_BYTE, .value = {.ts_int = -4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_BYTE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_BYTE);
        REQUIRE(result->value.ts_int == 4);
        REQUIRE(return_value == 1);
    } //
    SECTION("TS_SHORT") {
        const struct ts_value op1 = {.type = TS_SHORT, .value = {.ts_int = -4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_SHORT, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_SHORT);
        REQUIRE(result->value.ts_int == 4);
        REQUIRE(return_value == 1);
    } //
    SECTION("TS_INTEGER") {
        const struct ts_value op1 = {.type = TS_INTEGER, .value = {.ts_int = -4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_INTEGER, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_INTEGER);
        REQUIRE(result->value.ts_int == 4);
        REQUIRE(return_value == 1);
    } //
    SECTION("TS_LONG") {
        const struct ts_value op1 = {.type = TS_LONG, .value = {.ts_long = -4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_LONG, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_LONG);
        REQUIRE(result->value.ts_long == 4);
        REQUIRE(return_value == 1);
    } //
    SECTION("TS_UNSIGNED_BYTE") {
        const struct ts_value op1 = {.type = TS_UNSIGNED_BYTE, .value = {.ts_unsigned_int = 4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNSIGNED_BYTE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_UNSIGNED_BYTE);
        REQUIRE(result->value.ts_unsigned_int == 4);
        REQUIRE(return_value == 1);
    } //
    SECTION("TS_UNSIGNED_SHORT") {
        const struct ts_value op1 = {.type = TS_UNSIGNED_SHORT, .value = {.ts_unsigned_int = 4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNSIGNED_SHORT, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_UNSIGNED_SHORT);
        REQUIRE(result->value.ts_unsigned_int == 4);
        REQUIRE(return_value == 1);
    } //
    SECTION("TS_UNSIGNED_INTEGER") {
        const struct ts_value op1 = {.type = TS_UNSIGNED_INTEGER, .value = {.ts_unsigned_int = 4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value = df_arithmetic_operation_with_type(
            arithmetic_operation, op_list, operand_count, TS_UNSIGNED_INTEGER, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_UNSIGNED_INTEGER);
        REQUIRE(result->value.ts_unsigned_int == 4);
        REQUIRE(return_value == 1);
    } //
    SECTION("TS_UNSIGNED_LONG") {
        const struct ts_value op1 = {.type = TS_UNSIGNED_LONG, .value = {.ts_unsigned_long = 4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNSIGNED_LONG, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_UNSIGNED_LONG);
        REQUIRE(result->value.ts_unsigned_long == 4);
        REQUIRE(return_value == 1);
    } //
    SECTION("TS_DOUBLE") {
        const struct ts_value op1 = {.type = TS_DOUBLE, .value = {.ts_double = -4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_DOUBLE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_DOUBLE);
        REQUIRE(result->value.ts_double == 4);
        REQUIRE(return_value == 1);
    } //
    SECTION("TS_DATETIME") {
        const struct ts_value op1 = {.type = TS_TIMESTAMP, .value = {.ts_long = -4}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_TIMESTAMP, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_STRING") {
        const struct ts_value op1 = {.type = TS_STRING, .value = {.ts_string = {}}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_STRING, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_ARRAY") {
        const struct ts_value op1 = {.type = TS_ARRAY, .value = {.ts_array = {}}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_ARRAY, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_LIST") {
        const struct ts_value op1 = {.type = TS_LIST, .value = {.ts_list = {}}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_LIST, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_QUEUE") {
        const struct ts_value op1 = {.type = TS_QUEUE, .value = {.ts_queue = {}}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_QUEUE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_RECORD") {
        const struct ts_value op1 = {.type = TS_RECORD, .value = {.ts_record = {}}};
        const struct ts_value* op_list[1] = {&op1};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_RECORD, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
}

TEST_CASE("arithmetic: abs: two values") {
    const enum df_arithmetic_ops arithmetic_operation = DF_ARITH_ABS;
    const int operand_count = 2;
    struct ts_value* result;
    int return_value;
    SECTION("TS_UNINITIALIZED") {
        const struct ts_value op1 = {.type = TS_UNINITIALIZED, .value = {}};
        const struct ts_value op2 = {.type = TS_UNINITIALIZED, .value = {}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNINITIALIZED, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_BYTE") {
        const struct ts_value op1 = {.type = TS_BYTE, .value = {.ts_int = 4}};
        const struct ts_value op2 = {.type = TS_BYTE, .value = {.ts_int = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_BYTE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_BYTE);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_SHORT") {
        const struct ts_value op1 = {.type = TS_SHORT, .value = {.ts_int = 4}};
        const struct ts_value op2 = {.type = TS_SHORT, .value = {.ts_int = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_SHORT, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_SHORT);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_INTEGER") {
        const struct ts_value op1 = {.type = TS_INTEGER, .value = {.ts_int = 4}};
        const struct ts_value op2 = {.type = TS_INTEGER, .value = {.ts_int = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_INTEGER, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_INTEGER);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_LONG") {
        const struct ts_value op1 = {.type = TS_LONG, .value = {.ts_long = 4}};
        const struct ts_value op2 = {.type = TS_LONG, .value = {.ts_long = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_LONG, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_LONG);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_UNSIGNED_BYTE") {
        const struct ts_value op1 = {.type = TS_UNSIGNED_BYTE, .value = {.ts_unsigned_int = 4}};
        const struct ts_value op2 = {.type = TS_UNSIGNED_BYTE, .value = {.ts_unsigned_int = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNSIGNED_BYTE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_UNSIGNED_BYTE);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_UNSIGNED_SHORT") {
        const struct ts_value op1 = {.type = TS_UNSIGNED_SHORT, .value = {.ts_unsigned_int = 4}};
        const struct ts_value op2 = {.type = TS_UNSIGNED_SHORT, .value = {.ts_unsigned_int = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNSIGNED_SHORT, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_UNSIGNED_SHORT);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_UNSIGNED_INTEGER") {
        const struct ts_value op1 = {.type = TS_UNSIGNED_INTEGER, .value = {.ts_unsigned_int = 4}};
        const struct ts_value op2 = {.type = TS_UNSIGNED_INTEGER, .value = {.ts_unsigned_int = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value = df_arithmetic_operation_with_type(
            arithmetic_operation, op_list, operand_count, TS_UNSIGNED_INTEGER, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_UNSIGNED_INTEGER);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_UNSIGNED_LONG") {
        const struct ts_value op1 = {.type = TS_UNSIGNED_LONG, .value = {.ts_unsigned_long = 4}};
        const struct ts_value op2 = {.type = TS_UNSIGNED_LONG, .value = {.ts_unsigned_long = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_UNSIGNED_LONG, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_UNSIGNED_LONG);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_DOUBLE") {
        const struct ts_value op1 = {.type = TS_DOUBLE, .value = {.ts_double = 4}};
        const struct ts_value op2 = {.type = TS_DOUBLE, .value = {.ts_double = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_DOUBLE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(result->type == TS_DOUBLE);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_DATETIME") {
        const struct ts_value op1 = {.type = TS_TIMESTAMP, .value = {.ts_long = 4}};
        const struct ts_value op2 = {.type = TS_TIMESTAMP, .value = {.ts_long = 4}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_TIMESTAMP, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_STRING") {
        const struct ts_value op1 = {.type = TS_STRING, .value = {.ts_string = {}}};
        const struct ts_value op2 = {.type = TS_STRING, .value = {.ts_string = {}}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_STRING, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_ARRAY") {
        const struct ts_value op1 = {.type = TS_ARRAY, .value = {.ts_array = {}}};
        const struct ts_value op2 = {.type = TS_ARRAY, .value = {.ts_array = {}}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_ARRAY, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_LIST") {
        const struct ts_value op1 = {.type = TS_LIST, .value = {.ts_list = {}}};
        const struct ts_value op2 = {.type = TS_LIST, .value = {.ts_list = {}}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_LIST, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_QUEUE") {
        const struct ts_value op1 = {.type = TS_QUEUE, .value = {.ts_queue = {}}};
        const struct ts_value op2 = {.type = TS_QUEUE, .value = {.ts_queue = {}}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_QUEUE, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
    SECTION("TS_RECORD") {
        const struct ts_value op1 = {.type = TS_RECORD, .value = {.ts_record = {}}};
        const struct ts_value op2 = {.type = TS_RECORD, .value = {.ts_record = {}}};
        const struct ts_value* op_list[2] = {&op1, &op2};
        return_value =
            df_arithmetic_operation_with_type(arithmetic_operation, op_list, operand_count, TS_RECORD, &result);
        REQUIRE(&result != NULL);
        REQUIRE(return_value == 0);
    } //
}