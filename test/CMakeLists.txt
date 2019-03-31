# - Create Catch testing utility library
add_library(FLCatch STATIC catch.hpp catch-main.cpp)
target_compile_features(FLCatch PUBLIC cxx_std_11)
target_include_directories(FLCatch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# - Actual tests
add_executable(property_set_t property_set_t.cpp)
target_include_directories(property_set_t PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(property_set_t PRIVATE FLCatch Bayeux::Bayeux)
add_test(NAME property_set_t COMMAND property_set_t)
