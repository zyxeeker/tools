set(TEST test)

add_executable(${TEST} test.cpp)
add_executable(${TEST}_log test_log.cpp)
add_executable(${TEST}_thread test_thread.cpp)
add_executable(${TEST}_mem test_mem.cpp)
add_executable(${TEST}_cfg test_cfg.cpp)

target_link_libraries(${TEST}_log l${CMAKE_PROJECT_NAME})
target_link_libraries(${TEST}_thread l${CMAKE_PROJECT_NAME})
target_link_libraries(${TEST}_mem l${CMAKE_PROJECT_NAME})
target_link_libraries(${TEST}_cfg l${CMAKE_PROJECT_NAME} nlohmann_json::nlohmann_json)
