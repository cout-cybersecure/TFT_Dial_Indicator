cmake_minimum_required(VERSION 3.22)
project(tft_dial_realtime LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network WebSockets)

if(NOT Torch_DIR AND EXISTS "$ENV{HOME}/libtorch/share/cmake/Torch")
  set(Torch_DIR "$ENV{HOME}/libtorch/share/cmake/Torch")
endif()
if(NOT Torch_DIR AND DEFINED ENV{Torch_DIR})
  set(Torch_DIR $ENV{Torch_DIR})
endif()
if(Torch_DIR)
  list(APPEND CMAKE_PREFIX_PATH "${Torch_DIR}")
endif()
find_package(Torch REQUIRED)

if(DEFINED TORCH_INSTALL_PREFIX)
  set(CMAKE_BUILD_RPATH "${TORCH_INSTALL_PREFIX}/lib")
  set(CMAKE_INSTALL_RPATH "${TORCH_INSTALL_PREFIX}/lib")
elseif(EXISTS "$ENV{HOME}/libtorch/lib")
  set(CMAKE_BUILD_RPATH "$ENV{HOME}/libtorch/lib")
  set(CMAKE_INSTALL_RPATH "$ENV{HOME}/libtorch/lib")
endif()

add_executable(tft_dial_realtime
    cpp/main.cpp
    cpp/main_window.cpp
    cpp/main_window.h
    cpp/dial_widget.cpp
    cpp/dial_widget.h
    cpp/trainer_invoker.cpp
    cpp/trainer_invoker.h
    cpp/data_fetcher.cpp
    cpp/data_fetcher.h
    cpp/inference_engine.cpp
    cpp/inference_engine.h
)

target_include_directories(tft_dial_realtime PRIVATE cpp)
target_link_libraries(tft_dial_realtime PRIVATE Qt6::Widgets Qt6::Network Qt6::WebSockets "${TORCH_LIBRARIES}")
set_property(TARGET tft_dial_realtime PROPERTY CXX_STANDARD 17)

add_custom_target(copy_python ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/python ${CMAKE_BINARY_DIR}/python
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
)
add_dependencies(tft_dial_realtime copy_python)
