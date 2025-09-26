
#include "df_custom_operations.h"

#ifdef ESP8266
#include "ks_test.h"
#include "regress_test.h"
#include "corr_test.h"
#include "anomaly_test.h"
#else
#include "logger_system/df_operation_logger.h"
#include "type_system/types/ts_matrix.h"
#include "riot_benchmark/riot.h"
#include "mapreduce_benchmark/mapreduce.h"
#include "ks_benchmark/ks_test.h"
#include "regress_benchmark/regress_test.h"
#include "corr_benchmark/corr_test.h"
#include "anomaly_benchmark/anomaly_test.h"
#endif

#include <chrono>

int pass_through(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    value_deep_set(operands[0], result);
    return true;
}
int constant_true(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    result->type = TS_BOOLEAN;
    result->value.ts_int = true;
    return true;
}
int constant_false(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    result->type = TS_BOOLEAN;
    result->value.ts_int = false;
    return true;
}
int constant_0(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    result->type = TS_INTEGER;
    result->value.ts_int = 0;
    return true;
}
int constant_1(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    result->type = TS_INTEGER;
    result->value.ts_int = 1;
    return true;
}
int constant_minus_1(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    result->type = TS_INTEGER;
    result->value.ts_int = -1;
    return true;
}
int constant_0_0(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    result->type = TS_DOUBLE;
    result->value.ts_double = 0.0;
    return true;
}
int constant_1_0(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    result->type = TS_DOUBLE;
    result->value.ts_double = 1.0;
    return true;
}
int constant_minus_1_0(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    result->type = TS_DOUBLE;
    result->value.ts_double = -1.0;
    return true;
}
int math_max(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    if (result_type == TS_INTEGER) {
        result->type = TS_INTEGER;
        result->value.ts_int = operands[0]->value.ts_int;
        for (unsigned int i = 1; i < operand_count; i++) {
            if (operands[i]->type != TS_INTEGER) {
                log_type_mismatch(TS_INTEGER, operands[i]->type);
                return false;
            }
            if (operands[i]->value.ts_int > result->value.ts_int) {
                result->value.ts_int = operands[i]->value.ts_int;
            }
        }
    } else if (result_type == TS_DOUBLE) {
        result->type = TS_DOUBLE;
        result->value.ts_double = operands[0]->value.ts_double;
        for (unsigned int i = 1; i < operand_count; i++) {
            if (operands[i]->type != TS_DOUBLE) {
                log_type_mismatch(TS_DOUBLE, operands[i]->type);
                return false;
            }
            if (operands[i]->value.ts_double > result->value.ts_double) {
                result->value.ts_double = operands[i]->value.ts_double;
            }
        }
    } else {
        log_error("Not Implemented");
        return false;
    }
    return true;
}

int math_min(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {
    if (result_type == TS_INTEGER) {
        result->type = TS_INTEGER;
        result->value.ts_int = operands[0]->value.ts_int;
        for (unsigned int i = 1; i < operand_count; i++) {
            if (operands[i]->type != TS_INTEGER) {
                log_type_mismatch(TS_INTEGER, operands[i]->type);
                return false;
            }
            if (operands[i]->value.ts_int < result->value.ts_int) {
                result->value.ts_int = operands[i]->value.ts_int;
            }
        }
    } else if (result_type == TS_DOUBLE) {
        result->type = TS_DOUBLE;
        result->value.ts_double = operands[0]->value.ts_double;
        for (unsigned int i = 1; i < operand_count; i++) {
            if (operands[i]->type != TS_DOUBLE) {
                log_type_mismatch(TS_DOUBLE, operands[i]->type);
                return false;
            }
            if (operands[i]->value.ts_double < result->value.ts_double) {
                result->value.ts_double = operands[i]->value.ts_double;
            }
        }
    } else {
        log_error("Not Implemented");
        return false;
    }
    return true;
}

#ifndef ESP8266
int compute_sum_of_squares(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {

    int result_value = 0;
    int temp_value = 0;
    for (unsigned int i = 0; i < operand_count; i++) {
        temp_value = operands[i]->value.ts_int;
        result_value += temp_value * temp_value;
    }
    result->value.ts_int = result_value;
    return 1;
}

#define MATMUL_TIMING
int vector_dot_product(const struct ts_value* const* operands,
                       const unsigned int operand_count,
                       const enum ts_types result_type,
                       struct ts_value* const result) {

    // Get arrays
    int* vec_a = value_to_integer_array(operands[0]);
    int* vec_b = value_to_integer_array(operands[1]);
    size_t size = get_array_size(operands[0]);

    // Perform dot product
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += vec_a[i] * vec_b[i];
    }
    free(vec_a);
    free(vec_b);
    log_debug("Vector sum: %d", sum);

#ifndef MATMUL_TIMING
    set_integer(result, sum);
#endif

    // Add time of completion to output instead of result
#ifdef MATMUL_TIMING
    auto now = std::chrono::high_resolution_clock::now();
    auto ts = std::chrono::duration_cast<std::chrono::nanoseconds>(
                          now.time_since_epoch())
                          .count();
    set_long(result, ts);
#endif

    return true;
}

int matrix_multiply(const struct ts_value* const* operands,
                    const unsigned int operand_count,
                    const enum ts_types result_type,
                    struct ts_value* const result) {

    // Get matrix dimensions
    const size_t mat_a_rows = get_row_count(operands[0]);
    const size_t mat_a_cols = get_column_count(operands[0]);
    const size_t mat_b_rows = get_row_count(operands[1]);
    const size_t mat_b_cols = get_column_count(operands[1]);

    // Get matrices
    int32_t mat_a[mat_a_rows][mat_a_cols];
    get_integer_matrix(&mat_a, operands[0]);
    int32_t mat_b[mat_b_rows][mat_b_cols];
    get_integer_matrix(&mat_b, operands[1]);

    // Create result
    int32_t mat_r[mat_a_rows][mat_b_cols] = {};
    
    // Perform matrix multiplication
    for (int i = 0; i < mat_a_rows; i++) {
        for (int j = 0; j < mat_b_cols; j++) {
            for (int k = 0; k < mat_a_cols; k++) {
                mat_r[i][j] += (mat_a[i][k] * mat_b[k][j]);
            }
        }
    }

    set_integer_matrix(result, mat_r, mat_a_rows, mat_b_cols);

    return true;
}

int matrix_multiply_timing(const struct ts_value* const* operands,
                           const unsigned int operand_count,
                           const enum ts_types result_type,
                           struct ts_value* const result) {

    // Get matrix dimensions
    const size_t mat_a_rows = get_row_count(operands[0]);
    const size_t mat_a_cols = get_column_count(operands[0]);
    const size_t mat_b_rows = get_row_count(operands[1]);
    const size_t mat_b_cols = get_column_count(operands[1]);

    // Get matrices
    int32_t mat_a[mat_a_rows][mat_a_cols];
    get_integer_matrix(&mat_a, operands[0]);
    int32_t mat_b[mat_b_rows][mat_b_cols];
    get_integer_matrix(&mat_b, operands[1]);

    // Create result
    int32_t mat_r[mat_a_rows][mat_b_cols] = {};
    
    // Perform matrix multiplication
    for (int i = 0; i < mat_a_rows; i++) {
        for (int j = 0; j < mat_b_cols; j++) {
            for (int k = 0; k < mat_a_cols; k++) {
                mat_r[i][j] += (mat_a[i][k] * mat_b[k][j]);
            }
        }
    }

    // Add time of completion to output instead of result
    auto now = std::chrono::high_resolution_clock::now();
    auto ts = std::chrono::duration_cast<std::chrono::nanoseconds>(
                          now.time_since_epoch())
                          .count();
    set_long(result, ts);

    return true;
}

#endif // ESP8266

int df_custom_operation(const enum df_custom_ops custom_operation,
                        const struct ts_value* const* operands,
                        const unsigned int operand_count,
                        struct df_operation_metadata* const operation_metadata,
                        const enum ts_types result_type,
                        struct ts_value** result) {
    auto* result_value = static_cast<ts_value*>(malloc(sizeof(struct ts_value)));
    result_value->type = result_type;
    *result = result_value;

    switch (custom_operation) {
        case PASS_THROUGH:
            return pass_through(operands, operand_count, result_type, result_value);
        case CONSTANT_TRUE:
            return constant_true(operands, operand_count, result_type, result_value);
        case CONSTANT_FALSE:
            return constant_false(operands, operand_count, result_type, result_value);
        case CONSTANT_0:
            return constant_0(operands, operand_count, result_type, result_value);
        case CONSTANT_1:
            return constant_1(operands, operand_count, result_type, result_value);
        case CONSTANT_MINUS_1:
            return constant_minus_1(operands, operand_count, result_type, result_value);
        case CONSTANT_0_0:
            return constant_0_0(operands, operand_count, result_type, result_value);
        case CONSTANT_1_0:
            return constant_1_0(operands, operand_count, result_type, result_value);
        case CONSTANT_MINUS_1_0:
            return constant_minus_1_0(operands, operand_count, result_type, result_value);
        case MATH_MAX:
            return math_max(operands, operand_count, result_type, result_value);
        case MATH_MIN:
            return math_min(operands, operand_count, result_type, result_value);
#ifndef ESP8266
        case SUM_OF_SQUARES:
            return compute_sum_of_squares(operands, operand_count, result_type, result_value);

        /* Matrix Multiplication */
        case VECTOR_DOT_PRODUCT:
            return vector_dot_product(operands, operand_count, result_type, result_value);
        case MATRIX_MULTIPLY:
            return matrix_multiply(operands, operand_count, result_type, result_value);
        case MATRIX_MULTIPLY_TIMING:
            return matrix_multiply_timing(operands, operand_count, result_type, result_value);
        
        /////////////////
        // Mapreduce Benchmark
        /////////////////
        case KMEANS_CLUTSERING_MAPPER_PRIM:
            return k_means_clustering_mapper_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case KMEANS_CLUTSERING_REDUCER_PRIM:
            return k_means_clustering_reducer_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case LOGISTIC_REGRESSION_MAPPER_PRIM:
            return logistic_regression_mapper_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case LOGISTIC_REGRESSION_REDUCER_PRIM:
            return logistic_regression_reducer_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case GREP_REGEX_MAPPER_PRIM:
            return grep_regex_mapper_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case GREP_WORD_COUNT_REDUCER_PRIM:
            return grep_word_count_reducer_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case GREP_INVERSE_MAPPER_PRIM:
            return grep_inverse_mapper_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case GREP_FREQUENCY_REDUCER_PRIM:
            return grep_frequency_reducer_prim(operands, operand_count, operation_metadata, result_type, result_value);

        /////////////////
        // RIoT Benchmark
        /////////////////

        /* ETL */
        case RIOT_SERIALIZE_SENML:
            return serialize_senml_node(operands, operand_count, result_type, result_value);
        case RIOT_SERIALIZE_SENML_PRIM:
            return serialize_senml_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_RANGE_FILTER:
            return range_filter_node(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_RANGE_FILTER_PRIM:
            return range_filter_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_BLOOM_FILTER:
            return bloom_filter_node(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_BLOOM_FILTER_PRIM:
            return bloom_filter_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_INTERPOLATE:
            return interpolate_node(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_INTERPOLATE_PRIM:
            return interpolate_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_ANNOTATE:
            return annotate_node(operands, operand_count, result_type, result_value);
        case RIOT_ANNOTATE_PRIM:
            return annotate_node_prim(operands, operand_count, result_type, result_value);

        /* STATS */
        case RIOT_AVERAGE:
            return average_node(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_AVERAGE_PRIM:
            return average_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_KALMAN_FILTER:
            return kalman_filter_node(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_KALMAN_FILTER_PRIM:
            return kalman_filter_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_DISTINCT_COUNT:
            return distinct_count_node(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_DISTINCT_COUNT_PRIM:
            return distinct_count_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_REGRESSION:
            return regression_node(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_WINDOW_REGRESSION_PRIM:
            return window_regression_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_PLOT:
            return plot_node(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_PLOT_PRIM:
            return plot_node_prim(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_PLOT_SETUP:
            return setup_plot_node(operands, operand_count, operation_metadata, result_type, result_value);
        case RIOT_SINK:
            return sink_node(operands, operand_count, operation_metadata, result_type, result_value);
#endif // ESP8266

	case KS_TEST:
            return ks_test_node(operands, operand_count, operation_metadata, result_type, result_value);
	case REGRESS_TEST:
            return regress_test_node(operands, operand_count, operation_metadata, result_type, result_value);
	case CORR_TEST:
            return corr_test_node(operands, operand_count, operation_metadata, result_type, result_value);
	case ANOMALY_TEST:
            return anomaly_test_node(operands, operand_count, operation_metadata, result_type, result_value);
	case ANOMALY_PROXY:
            return anomaly_proxy_node(operands, operand_count, operation_metadata, result_type, result_value);
	case ANOMALY_OPT_SRC:
            return anomaly_optsrc_node(operands, operand_count, operation_metadata, result_type, result_value);
	case ANOMALY_OPT_SNK:
            return anomaly_optsnk_node(operands, operand_count, operation_metadata, result_type, result_value);
        default:
#ifndef ESP8266
            log_operation_not_existing("CUSTOM_OPERATION", custom_operation);
#else
            printf("CUSTOM_OPERATION %d not recognized\n",(int)custom_operation);
#endif

            return 0;
    }
}
