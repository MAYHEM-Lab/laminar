//
// Created by Lukas Brand on 29.03.23.
//

#ifndef CSPOT_DF_OPERATIONS_H
#define CSPOT_DF_OPERATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

enum df_operation_categories {
    DF_CAST,
    DF_LOGIC,
    DF_ARITHMETIC,
    DF_INTERNAL,
    DF_MACHINE_LEARNING,
    DF_LINEAR_ALGEBRA,
    DF_STRING,
    DF_CUSTOM
};

struct df_operation {
    enum df_operation_categories category;
    unsigned int operation;
};

struct df_flow_control {
    unsigned long execution_iteration;
    bool write_value;
    const char* output_woof_path;
};

struct df_operation_metadata {
    const int host_id;
    const int node_namespace;
    const int node_id;
    struct df_flow_control flow_control;
};


enum df_cast_ops {
    DF_CAST_TO_BOOLEAN,
    DF_CAST_TO_BYTE,
    DF_CAST_TO_SHORT,
    DF_CAST_TO_INTEGER,
    DF_CAST_TO_LONG,
    DF_CAST_TO_UNSIGNED_BYTE,
    DF_CAST_TO_UNSIGNED_SHORT,
    DF_CAST_TO_UNSIGNED_INTEGER,
    DF_CAST_TO_UNSIGNED_LONG,
    DF_CAST_TO_FLOAT,
    DF_CAST_TO_DOUBLE,
};

enum df_logic_ops {
    DF_LOGIC_NOT,
    DF_LOGIC_AND,
    DF_LOGIC_OR,
    DF_LOGIC_LESS_THAN,
    DF_LOGIC_LESS_THAN_EQUALS,
    DF_LOGIC_GREATER_THAN,
    DF_LOGIC_GREATER_THAN_EQUALS,
    DF_LOGIC_EQUALS,
    DF_LOGIC_NOT_EQUALS,
};

enum df_arithmetic_ops {
    DF_ARITH_ADDITION,
    DF_ARITH_SUBTRACTION,
    DF_ARITH_MULTIPLICATION,
    DF_ARITH_DIVISION,
    DF_ARITH_MODULO,
    DF_ARITH_SQRT,
    DF_ARITH_ABS,
};

enum df_internal_ops {
    DF_INTERNAL_NOOP,
    DF_INTERNAL_OPERAND,
    DF_INTERNAL_SELECT,
    DF_INTERNAL_FILTER,
    DF_INTERNAL_OFFSET,
    DF_INTERNAL_FLATTEN
};

enum df_machine_learning_ops {
    DF_MACHINE_LEARNING_KNN,
    DF_MACHINE_LEARNING_LINREG,
};

enum df_linear_algebra_ops {
    DF_LINEAR_ALGEBRA_MATRIX_IS_MULTIPLIABLE,
    DF_LINEAR_ALGEBRA_MATRIX_MULTIPLICATION
};

enum df_string_ops {
    DF_STRING_SPLIT
};

enum df_custom_ops {
    /* Pass Through (important for sinks: safe operation, that doesn't change the result) */
    PASS_THROUGH,

    /* Constants */
    CONSTANT_TRUE,
    CONSTANT_FALSE,
    CONSTANT_0,
    CONSTANT_1,
    CONSTANT_MINUS_1,
    CONSTANT_0_0,
    CONSTANT_1_0,
    CONSTANT_MINUS_1_0,

    SUM_OF_SQUARES,

    /* Matrix Multiplication*/
    VECTOR_DOT_PRODUCT,
    MATRIX_MULTIPLY,
    MATRIX_MULTIPLY_TIMING,

    /////////////////
    // Mapreduce Benchmark
    /////////////////
    KMEANS_CLUTSERING_MAPPER_PRIM,
    KMEANS_CLUTSERING_REDUCER_PRIM,
    LOGISTIC_REGRESSION_MAPPER_PRIM,
    LOGISTIC_REGRESSION_REDUCER_PRIM,
    GREP_REGEX_MAPPER_PRIM,
    GREP_WORD_COUNT_REDUCER_PRIM,
    GREP_INVERSE_MAPPER_PRIM,
    GREP_FREQUENCY_REDUCER_PRIM,

    /////////////////
    // RIoT Benchmark
    /////////////////

    /* ETL */
    RIOT_SERIALIZE_SENML,
    RIOT_SERIALIZE_SENML_PRIM,
    RIOT_RANGE_FILTER,
    RIOT_RANGE_FILTER_PRIM,
    RIOT_BLOOM_FILTER,
    RIOT_BLOOM_FILTER_PRIM,
    RIOT_INTERPOLATE,
    RIOT_INTERPOLATE_PRIM,
    RIOT_ANNOTATE,
    RIOT_ANNOTATE_PRIM,

    /* STATS */
    RIOT_AVERAGE,
    RIOT_AVERAGE_PRIM,
    RIOT_KALMAN_FILTER,
    RIOT_KALMAN_FILTER_PRIM,
    RIOT_DISTINCT_COUNT,
    RIOT_DISTINCT_COUNT_PRIM,
    RIOT_REGRESSION,
    RIOT_WINDOW_REGRESSION_PRIM,
    RIOT_PLOT,
    RIOT_PLOT_PRIM,
    RIOT_PLOT_SETUP,
    RIOT_SINK,

    ///////////////////////////////////
    // KS anomaly detector benchmark //
    ///////////////////////////////////
    KS_TEST,
    REGRESS_TEST,
    CORR_TEST,
    ANOMALY_TEST,
    ANOMALY_PROXY,
    ANOMALY_OPT_SRC,
    ANOMALY_OPT_SNK
};

char* operation_to_string(struct df_operation type);

#ifdef __cplusplus
}
#endif

#endif // CSPOT_DF_OPERATIONS_H
