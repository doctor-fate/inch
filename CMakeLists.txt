cmake_minimum_required(VERSION 3.5)
project(inch)

find_package(Clang REQUIRED CONFIG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11 -fno-rtti")

set(SOURCE_FILES main.cpp Position.cpp Position.h
        PositionManager.cpp PositionManager.h BaseChecker.cpp BaseChecker.h DeclIndentsChecker.h
        StmtIndentsChecker.h DeclIndentsChecker.cpp StmtIndentsChecker.cpp)

add_executable(inch ${SOURCE_FILES})

target_link_libraries(inch clangTooling)