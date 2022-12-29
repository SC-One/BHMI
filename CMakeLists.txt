cmake_minimum_required(VERSION 3.5)

project(BHMI LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Core SerialPort Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

add_executable(BHMI
  main.cpp
  BHMI.h BHMI.cpp BHMI.ui
  BucketsModel.h BucketsModel.cpp
  Structures.h
  NewBucket.h NewBucket.cpp NewBucket.ui
  DriverSettings.h DriverSettings.cpp DriverSettings.ui
  SerialHandler.h SerialHandler.cpp SerialHandler.ui
  ApplicationTime.h ApplicationTime.cpp ApplicationTime.ui
  CameraDriver.h CameraDriver.cpp
  PrintFirst.h PrintFirst.cpp
)

target_link_libraries(BHMI PRIVATE
    Qt5::Widgets
    Qt5::SerialPort
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::PrintSupport
#    i2c
)

#message(FATAL_ERROR ${I2C_FOUND})


##[=======================================================================[.rst:
#FindI2C
#-------

#Find the I2C device library.

#Once done this will define the following variables::

#  I2C_INCLUDE_DIRS    - I2C include directory
#  I2C_LIBRARIES       - I2C libraries
#  I2C_FOUND           - if false, you cannot build anything that requires I2C
##]=======================================================================]
