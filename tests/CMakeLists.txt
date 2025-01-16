set(TEST test)

enable_testing()

find_package(GTest CONFIG REQUIRED)

add_executable(${TEST} test.cpp)
add_executable(${TEST}_log test_log.cpp)
add_executable(${TEST}_thread test_thread.cpp)
add_executable(${TEST}_mem test_mem.cpp)
add_executable(${TEST}_cfg test_cfg.cpp)
add_executable(${TEST}_pattern test_pattern.cpp)
add_executable(${TEST}_var test_var.cpp)
add_executable(${TEST}_cvt test_cvt.cpp)
add_executable(${TEST}_rwlock test_rwlock.cpp)

target_link_libraries(${TEST}_log l${CMAKE_PROJECT_NAME})
target_link_libraries(${TEST}_thread l${CMAKE_PROJECT_NAME})
target_link_libraries(${TEST}_mem l${CMAKE_PROJECT_NAME})
target_link_libraries(${TEST}_cfg l${CMAKE_PROJECT_NAME} nlohmann_json::nlohmann_json)
target_link_libraries(${TEST}_pattern l${CMAKE_PROJECT_NAME})
target_link_libraries(${TEST}_var l${CMAKE_PROJECT_NAME} nlohmann_json::nlohmann_json)
target_link_libraries(${TEST}_cvt PRIVATE l${CMAKE_PROJECT_NAME} GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
