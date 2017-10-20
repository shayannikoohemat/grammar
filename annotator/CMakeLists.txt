cmake_minimum_required(VERSION 3.6)
project(annotator)


set(CMAKE_CXX_STANDARD 11)

set(dir  "D:/Mapping/Tools/grammar/annotator/Debug")  # Release or Debug
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir})

set (MAPPING_LIB_DIR "D:/Mapping/Library/lib64")
set (MAPPING_INCLUDE_DIR "D:/Mapping/Library")

# for boost library
set(Boost_INCLUDE_DIR C:/Program\ Files/boost_gcc4.7/include/boost-1_64)
set(Boost_LIBRARY_DIR C:/Program\ Files/boost_gcc4.7/lib)
find_package(Boost COMPONENTS system filesystem regex REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

#include_directories(${MAPPING_INCLUDE_DIR}/Photogrammetry/include)
include_directories(D:/Mapping/Library/Photogrammetry/include)
include_directories(D:/Mapping/Library/Buildings/include)
include_directories(D:/Mapping/Library/LaserScan/include)
include_directories(D:/Mapping/Foreign/ANN/include/ANN)
include_directories(D:/Mapping/Foreign/ANN/src)
#link_directories(D:/Mapping/Library/lib64)
link_directories(${MAPPING_LIB_DIR})
link_libraries(-lLaserScan -lPhotogrammetry -lBuildings -lEispack -lLinpack
                -lANN -lnewmat -lgfortran -g3 -llas)

set(SOURCE_FILES main.cpp Annotator.h AnnotatedPoint.h Annotator.cpp AnnotatedPoints.h
        AnnotationManipulator.cpp laser2ascii_cpp.cc readAsciiPCD.cpp)
add_executable(annotator ${SOURCE_FILES})
target_link_libraries(annotator ${Boost_LIBRARIES})