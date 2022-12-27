cmake_minimum_required(VERSION 3.5)

project(BHMI LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(BHMI
  main.cpp
  BHMI.h BHMI.cpp BHMI.ui
  BucketsModel.h BucketsModel.cpp
  Structures.h
  NewBucket.h NewBucket.cpp NewBucket.ui
)

target_link_libraries(BHMI PRIVATE Qt5::Widgets )
