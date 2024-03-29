set(SRC ${CMAKE_PROJECT_NAME})

set(LIB_NAME l${SRC})

file(GLOB_RECURSE SOURCES *.h *.cpp)

add_library(${LIB_NAME} STATIC ${SOURCES})
