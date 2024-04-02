set(TEST test)

add_executable(${TEST} test.cpp)
add_executable(${TEST}_log test_log.cpp)

target_link_libraries(${TEST}_log l${CMAKE_PROJECT_NAME})
