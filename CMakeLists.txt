cmake_minimum_required(VERSION 3.3)
project(FalaiseProcessing VERSION 0.1.0)

find_package(Falaise REQUIRED)

add_library(MockTrackerCalibrator SHARED MockTrackerCalibrator.cpp)
target_link_libraries(MockTrackerCalibrator Falaise::FalaiseModule)
