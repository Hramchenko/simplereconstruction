cmake_minimum_required(VERSION 3.5)
project(SimpleReconstruction LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

set(OpenCV_DIR "/home/user/ML/VENV/opt/opencv-4.2.0/install/lib/cmake/opencv4/")
find_package(OpenCV REQUIRED)
include_directories(${OPENCV_INCLUDE_DIRECTORIES})

find_package(OpenGL)

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
file(GLOB HEADERS_LIST ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

add_executable(SimpleReconstruction ${SRC_LIST} ${HEADERS_LIST})
target_link_libraries(SimpleReconstruction PRIVATE ${OPENGL_LIBRARIES} ${OpenCV_LIBS} Qt5::Widgets Qt5::OpenGL)






