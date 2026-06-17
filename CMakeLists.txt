# ============================================================================
# CMakeLists.txt —— 可选 CMake 构建脚本
# ----------------------------------------------------------------------------
# 用法：
#   cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64/lib/cmake"
#   cmake --build build --config Release
# ============================================================================

cmake_minimum_required(VERSION 3.16)
project(ndd-strings-to-sqlist LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_definitions(-DNDD_EXPORTDLL -D_UNICODE -DUNICODE)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

add_library(${PROJECT_NAME} SHARED
    src/StringsToSqlListPlugin.h
    src/StringsToSqlListPlugin.cpp
    src/AdvancedDialog.h
    src/AdvancedDialog.cpp
    src/qttestclass.h
    src/qttestclass.cpp
    src/pluginGl.h
    src/nddpluginapi.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/Qsci
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    # 以下链接库按本地情况启用（需与 NDD 主程序版本一致）
    # ${CMAKE_CURRENT_SOURCE_DIR}/libs/qmyedit_qt5.lib
    # ${CMAKE_CURRENT_SOURCE_DIR}/libs/qscintilla2_qt5.lib
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
