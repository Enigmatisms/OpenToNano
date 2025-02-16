cmake_minimum_required(VERSION 3.15)
project(vdb2nvdb_converter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找必需依赖
list(APPEND CMAKE_MODULE_PATH /home/hqy/graphics/openvdb-11.0.0/cmake)
find_package(OpenVDB REQUIRED COMPONENTS openvdb nanovdb)
find_package(Blosc REQUIRED)
find_package(TBB REQUIRED)

set(NANOVDB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/nanovdb" CACHE PATH "Path to NanoVDB headers")
find_path(NANOVDB_INCLUDE_DIR
    NAMES nanovdb/util/CreateNanoGrid.h
    HINTS ${NANOVDB_ROOT}
    PATH_SUFFIXES include
)

if(NOT NANOVDB_INCLUDE_DIR)
    message(FATAL_ERROR "NanoVDB headers not found. Please set NANOVDB_ROOT to NanoVDB installation path")
endif()

add_executable(vdb2nvdb
    src/main.cpp
)

target_include_directories(vdb2nvdb PRIVATE
    ${NANOVDB_INCLUDE_DIR}
    ${OpenVDB_INCLUDE_DIRS}
)

target_link_libraries(vdb2nvdb PRIVATE
    OpenVDB::openvdb
    TBB::tbb
    Blosc::blosc
    ${CMAKE_DL_LIBS}
)

set_target_properties(vdb2nvdb PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

target_compile_definitions(vdb2nvdb
    PRIVATE
    NANOVDB_USE_OPENVDB
)

install(TARGETS vdb2nvdb
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)