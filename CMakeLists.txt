cmake_minimum_required(VERSION 3.10)
project(DiffurchTests)
set(CMAKE_CXX_STANDARD 23)


# set(SOURCES, 
#   test/global_error.cpp,
#   test/state_symbols.cpp
# )

execute_process(
    COMMAND python3 -c "import sysconfig; print(sysconfig.get_path('include'))"
    OUTPUT_VARIABLE PYTHON_INCLUDE_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND python3 -c "import numpy; print(numpy.get_include())"
    OUTPUT_VARIABLE PYTHON_NUMPY_INCLUDE_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND python3 -c "import sysconfig; print(sysconfig.get_config_var('LDLIBRARY')[3:-3])"
    OUTPUT_VARIABLE PYTHON_LIBRARY_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# foreach(SOURCE ${SOURCES})
#     # Extract file name without extension to use as the executable name
#     get_filename_component(EXE_NAME ${SOURCE} NAME_WE)
#     add_executable(${EXE_NAME} ${SOURCE})
# endforeach()


add_executable(my_program test/global_error.cpp)
target_include_directories(my_program PRIVATE ${PYTHON_INCLUDE_PATH} ${PYTHON_NUMPY_INCLUDE_PATH})

target_link_libraries(my_program PRIVATE ${PYTHON_LIBRARY_PATH})



