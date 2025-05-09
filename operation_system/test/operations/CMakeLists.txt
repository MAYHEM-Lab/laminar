include(FetchContent)
fetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.3.2
)
fetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_executable(df_test_arithmetic
        df_test_arithmetic_addition.cpp
        df_test_arithmetic_subtraction.cpp
        df_test_arithmetic_multiplication.cpp
        df_test_arithmetic_division.cpp
        df_test_arithmetic_modulo.cpp
        df_test_arithmetic_sqrt.cpp
        df_test_arithmetic_abs.cpp)
target_link_libraries(df_test_arithmetic
        PRIVATE Catch2::Catch2WithMain
        PRIVATE ts_type
        PRIVATE df_operation
        PRIVATE df_logger
        PRIVATE df_interface)
catch_discover_tests(df_test_arithmetic)
target_compile_features(df_test_arithmetic PUBLIC cxx_std_17)

add_executable(df_test_logic
        df_test_logic_lt.cpp
        df_test_logic_lte.cpp
        df_test_logic_gt.cpp
        df_test_logic_gte.cpp
        df_test_logic_eq.cpp
        df_test_logic_neq.cpp
        df_test_logic_not.cpp
        df_test_logic_and.cpp
        df_test_logic_or.cpp)
target_link_libraries(df_test_logic
        PRIVATE Catch2::Catch2WithMain
        PRIVATE ts_type
        PRIVATE df_operation
        PRIVATE df_logger
        PRIVATE df_interface)
catch_discover_tests(df_test_logic)
target_compile_features(df_test_logic PUBLIC cxx_std_17)

add_executable(df_test_linear_algebra
        df_test_linear_algebra_matrix_multiplication.cpp)
target_link_libraries(df_test_linear_algebra
        PRIVATE Catch2::Catch2WithMain
        PRIVATE ts_type
        PRIVATE df_operation
        PRIVATE df_logger
        PRIVATE df_interface
        PRIVATE ts_matrix)
catch_discover_tests(df_test_linear_algebra)
target_compile_features(df_test_linear_algebra PUBLIC cxx_std_17)

