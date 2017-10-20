cmake_minimum_required(VERSION 3.6)
project(scene_graph)

set(CMAKE_CXX_STANDARD 11)

set(dir  "D:/Mapping/Tools/grammar/scene_graph/Debug")  # Release or Debug
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir})

set (MAPPING_LIB_DIR "D:/Mapping/Library/lib64")
set (MAPPING_INCLUDE_DIR "D:/Mapping/Library")

#include_directories(${MAPPING_INCLUDE_DIR}/Photogrammetry/include)
include_directories(D:/Mapping/Library/Photogrammetry/include)
include_directories(D:/Mapping/Library/Buildings/include)
include_directories(D:/Mapping/Library/LaserScan/include)
include_directories(D:/Mapping/Foreign/ANN/include/ANN)
include_directories(D:/Mapping/Foreign/ANN/src)
#link_directories(D:/Mapping/Library/lib64)
link_directories(${MAPPING_LIB_DIR})
link_libraries(-lLaserScan -lPhotogrammetry -lBuildings -lEispack -lLinpack -lANN -lnewmat -lgfortran -g3 -llas)

set(SOURCE_FILES main.cpp)
add_executable(scene_graph ${SOURCE_FILES})