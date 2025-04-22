add_subdirectory(type_system)

add_executable(distributed_addition distributed_addition.cpp)
target_link_libraries(distributed_addition
        PRIVATE df_interface
        PRIVATE ts_type
        PRIVATE df_operation
        PRIVATE ts_array
        PRIVATE ts_matrix)

add_executable(distributed_two distributed_two.cpp)
target_link_libraries(distributed_two
        PRIVATE df_interface
        PRIVATE ts_type)

add_executable(distributed_vector distributed_vector.cpp)
target_link_libraries(distributed_vector
        PRIVATE df_interface
        PRIVATE ts_type
        PRIVATE df_operation
        PRIVATE ts_array
        PRIVATE ts_matrix
        PRIVATE ts_string)

add_executable(distributed_vector2 distributed_vector2.cpp)
target_link_libraries(distributed_vector2
        PRIVATE df_interface
        PRIVATE ts_type
        PRIVATE df_operation
        PRIVATE ts_array
        PRIVATE ts_matrix
        PRIVATE ts_string)

add_executable(multinode_regression multinode_regression.cpp)
target_link_libraries(multinode_regression
        PRIVATE df_interface
        PRIVATE ts_type
        PRIVATE df_operation)

add_executable(distributed_multinode_regression distributed_multinode_regression.cpp)
target_link_libraries(distributed_multinode_regression
        PRIVATE df_interface
        PRIVATE ts_type
        PRIVATE df_operation)

add_executable(crash_test crash_test.cpp)
target_link_libraries(crash_test
        PRIVATE df_interface
        PRIVATE ts_type)

add_executable(tests
        tests.cpp
        arithmetic_tests.cpp
        conditional_tests.cpp
        namespace_tests.cpp
        loop_tests.cpp
        types_tests.cpp)
target_link_libraries(tests
        PRIVATE df_interface
        PRIVATE ts_type
        PRIVATE df_operation
        PRIVATE ts_array
        PRIVATE ts_matrix
        PRIVATE ts_string)

add_executable(simple_laminar_example simple_laminar_example.cpp)
target_link_libraries(simple_laminar_example
        PRIVATE df_interface
        PRIVATE ts_type)

add_executable(distributed_simple_laminar_example distributed_simple_laminar_example.cpp)
target_link_libraries(distributed_simple_laminar_example
        PRIVATE df_interface
        PRIVATE ts_type)

add_executable(baseline-regression baseline-regression.cpp)

add_executable(custom_operation_test custom_operation_test.cpp)
target_link_libraries(custom_operation_test         
                PRIVATE df_interface
                PRIVATE ts_type
                PRIVATE df_operation
                PRIVATE ts_array)

add_executable(quadratic-preamble quadratic-preamble.cpp)
target_link_libraries(quadratic-preamble
        PRIVATE df_interface
        PRIVATE ts_type)

add_executable(quadratic-body quadratic-body.cpp)
target_link_libraries(quadratic-body
        PRIVATE df_interface
        PRIVATE ts_type)
