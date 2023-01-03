add_executable(example meshcat_example.cpp)
target_link_libraries(example ${PROJECT_NAME}::${PROJECT_NAME})
