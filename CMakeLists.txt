cmake_minimum_required(VERSION 3.3)
project(FalaiseProcessing VERSION 0.1.0)

find_package(Falaise)
find_package(Bayeux 3.3.1 REQUIRED)

if(Falaise_FOUND)
  add_library(MockTrackerCalibrator SHARED MockTrackerCalibrator.cpp)
  target_link_libraries(MockTrackerCalibrator Falaise::FalaiseModule)
endif()

enable_testing()
add_subdirectory(test)
