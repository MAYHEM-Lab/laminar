add_executable(ks_benchmark
        ks-test.cpp)
target_link_libraries(ks_benchmark
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)
add_executable(regress_benchmark
        regress-test.cpp)
target_link_libraries(regress_benchmark
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)
add_executable(anomaly_benchmark
        anomaly-test.cpp)
target_link_libraries(anomaly_benchmark
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)
add_executable(anomaly_setup
        anomaly-test-setup.cpp)
target_link_libraries(anomaly_setup
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)
add_executable(anomaly_body
        anomaly-test-body.cpp)
target_link_libraries(anomaly_body
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)
add_executable(anomaly_opt_setup
        anomaly-opt-setup.cpp)
target_link_libraries(anomaly_opt_setup
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)
add_executable(anomaly_opt_body
        anomaly-opt-body.cpp)
target_link_libraries(anomaly_opt_body
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)

add_executable(riot_benchmark
        riot.cpp)
target_link_libraries(riot_benchmark
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)

add_executable(dspbench_benchmark
        dspbench.cpp)
target_link_libraries(dspbench_benchmark
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)

add_executable(matmul
        matmul.cpp)
target_link_libraries(matmul
        PRIVATE df_interface
        PRIVATE ts_type
        PRIVATE df_operation
        PRIVATE ts_array
        PRIVATE ts_matrix)

add_executable(matmul-rich
        matmul-rich.cpp)
target_link_libraries(matmul-rich
        PRIVATE df_interface
        PRIVATE ts_type
        PRIVATE df_operation
        PRIVATE ts_array
        PRIVATE ts_matrix)

add_executable(k_means_mapreduce k_means_mapreduce.cpp)
target_link_libraries(k_means_mapreduce
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)

add_executable(logistic_regression_mapreduce logistic_regression_mapreduce.cpp)
target_link_libraries(logistic_regression_mapreduce
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)

add_executable(grep_mapreduce grep_mapreduce.cpp)
target_link_libraries(grep_mapreduce
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)

add_executable(riot_stats
        riot_stats.cpp)
target_link_libraries(riot_stats
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)

add_executable(msg_stress_test
        msg_stress_test.cpp)
target_link_libraries(msg_stress_test
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)
add_executable(woof_change_setup
        woof-change-setup.cpp)
target_link_libraries(woof_change_setup
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)
add_executable(woof_change_body
        woof-change-body.cpp)
target_link_libraries(woof_change_body
        PRIVATE df_interface
        PRIVATE df_operation
        PRIVATE ts_type)
