cmake_minimum_required(VERSION 3.9)
project(arima)

set(CMAKE_CXX_STANDARD 11)
aux_source_directory(. source_file)
add_executable(arima ${source_file} lib/lib_io.h lib/lib_time.h)
