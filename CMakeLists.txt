cmake_minimum_required(VERSION 3.10)
project(DiffurchTests)
set(CMAKE_CXX_STANDARD 23)


set(SOURCES
  examples/ndde_plot.cpp
  examples/harmonic_oscillator.cpp
  examples/lorenz.cpp
  examples/linear_ndde.cpp
  test/global_error.cpp
  test/event_detection.cpp
  test/piecewise.cpp
  test/api/events.cpp
  test/api/hist.cpp
  test/educational/constness.cpp
  test/educational/describe.cpp
)

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

foreach(SOURCE IN LISTS SOURCES)
    # Extract file name without extension to use as the executable name
    get_filename_component(EXE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXE_NAME} ${SOURCE})
    message(STATUS ${EXE_NAME})
    target_include_directories(${EXE_NAME} PRIVATE ${PYTHON_INCLUDE_PATH} ${PYTHON_NUMPY_INCLUDE_PATH})
    target_link_libraries(${EXE_NAME} PRIVATE ${PYTHON_LIBRARY_PATH} matplot)
endforeach()




