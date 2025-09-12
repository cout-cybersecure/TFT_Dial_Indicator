cmake_minimum_required(VERSION 3.16)

project(tft_dial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

include_directories(/home/technetium-addict/onnxruntime-linux-x64-1.21.0/include)
link_directories(/home/technetium-addict/onnxruntime-linux-x64-1.21.0/lib)

set(SOURCES
    cpp/main.cpp
    cpp/MainWindow.cpp
    cpp/DialWidget.cpp
    cpp/DataFetcher.cpp
    cpp/TrainerInvoker.cpp
    cpp/InferenceEngine.cpp
)

set(HEADERS
    cpp/MainWindow.h
    cpp/DialWidget.h
    cpp/DataFetcher.h
    cpp/TrainerInvoker.h
    cpp/InferenceEngine.h
)

add_executable(tft_dial
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(tft_dial
    PRIVATE
    Qt5::Widgets
    onnxruntime
)

target_include_directories(tft_dial PRIVATE cpp)
