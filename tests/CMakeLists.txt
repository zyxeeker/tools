set(TEST test)

add_executable(${TEST} test.cpp)

target_link_libraries(${TEST} l${CMAKE_PROJECT_NAME})
