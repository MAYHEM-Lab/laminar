cmake_minimum_required(VERSION 3.13)
project(laminar)

option(DEVICE "building cspot-device" NO)

set(CMAKE_CXX_STANDARD 20)

find_package(woof REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(benchmarks)
add_subdirectory(tests)
add_subdirectory(logger_system)
add_subdirectory(operation_system)
add_subdirectory(type_system)

add_library(df_interface
        df.h
        df_interface.cpp
        df_interface.h)
target_link_libraries(df_interface
        PRIVATE woof
        PRIVATE ts_type
        PUBLIC df_operation)

add_handler(output_handler output_handler.cpp)
target_link_libraries(output_handler
        PRIVATE df_interface
        PRIVATE df_logger
        PRIVATE ts_type)

add_handler(subscription_event_handler subscription_event_handler.cpp)
target_link_libraries(subscription_event_handler
        PRIVATE df_interface
        PRIVATE df_logger
        PRIVATE ts_type
        PRIVATE df_operation)

# Avoid searching for new glibc versions on Centos7
set(BUILD_SHARED_LIBS OFF)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "Emitting static binaries")
    #set(CMAKE_CXX_FLAGS “${CMAKE_CXX_FLAGS} -stdlib=libstdc++”)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,--gc-sections")
    set(CMAKE_C_FLAGS "-ffunction-sections -fdata-sections")
    set(CMAKE_CXX_FLAGS "-ffunction-sections -fdata-sections")
endif ()
