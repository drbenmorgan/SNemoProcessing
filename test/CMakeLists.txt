# - Create Catch testing utility library
add_library(FLCatch STATIC catch.hpp catch-main.cpp)
target_compile_features(FLCatch PUBLIC cxx_std_11)
target_include_directories(FLCatch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# - Actual tests
add_executable(property_set_t property_set_t.cpp)
target_include_directories(property_set_t PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(property_set_t PRIVATE FLCatch Bayeux::Bayeux)
add_test(NAME property_set_t COMMAND property_set_t)


add_executable(boost_units_t boost_units_t.cpp)
target_link_libraries(boost_units_t FLCatch Boost::boost)
add_test(NAME boost_units_t COMMAND boost_units_t)

# - Units
find_package(units REQUIRED)
add_executable(units_t units_t.cpp)
target_link_libraries(units_t PRIVATE FLCatch units::units)
target_compile_features(units_t PRIVATE cxx_std_14)

