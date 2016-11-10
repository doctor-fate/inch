cmake_minimum_required(VERSION 3.5.2)
project(inch)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fvisibility-inlines-hidden -std=c++11 \
-ffunction-sections -fdata-sections -fno-rtti -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS \
-D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

execute_process(COMMAND llvm-config --libdir OUTPUT_VARIABLE LIB_DIR)
string(STRIP ${LIB_DIR} LIB_DIR)
execute_process(COMMAND llvm-config --includedir OUTPUT_VARIABLE INCLUDE_DIR)
string(STRIP ${INCLUDE_DIR} INCLUDE_DIR)

link_directories(${LIB_DIR})
include_directories(${INCLUDE_DIR})

set(SOURCE_FILES main.cpp DeclIndentDefiner.cpp DeclIndentDefiner.h Position.cpp Position.h
        PositionManager.cpp PositionManager.h BaseChecker.cpp BaseChecker.h DeclIndentsChecker.h
        StmtIndentsChecker.h DeclIndentsChecker.cpp StmtIndentsChecker.cpp)

add_executable(inch ${SOURCE_FILES})

execute_process(COMMAND llvm-config --libs OUTPUT_VARIABLE LLVM_LIBS)
string(STRIP ${LLVM_LIBS} LLVM_LIBS)

set(CLANG_LIBS clangTooling clangFrontendTool clangFrontend clangDriver clangSerialization clangCodeGen clangParse
        clangSema clangStaticAnalyzerFrontend clangStaticAnalyzerCheckers clangStaticAnalyzerCore clangAnalysis
        clangARCMigrate clangRewrite clangRewriteFrontend clangEdit clangAST clangLex clangBasic clang)

execute_process(COMMAND llvm-config --system-libs OUTPUT_VARIABLE SYS_LIBS)
string(STRIP ${SYS_LIBS} SYS_LIBS)

target_link_libraries(inch ${CLANG_LIBS} ${LLVM_LIBS} ${SYS_LIBS})