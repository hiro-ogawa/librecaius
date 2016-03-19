cmake_minimum_required(VERSION 2.8)
project (librecaius_test)

find_package(Boost REQUIRED)

add_executable(librecaius_test librecaius_test.cpp librecaius.cpp)
target_link_libraries(librecaius_test curl)
target_link_libraries(librecaius_test jsoncpp)

add_custom_target(run ./librecaius_test DEPENDS librecaius_test)
