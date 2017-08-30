cmake_minimum_required(VERSION 3.2)
project(itkcvbf)

find_package(ITK 4.9 REQUIRED)
include(${ITK_USE_FILE})

find_package(OpenCV 3.3 REQUIRED)

add_library(itkcvbf INTERFACE)

target_sources(itkcvbf INTERFACE
    ${CMAKE_SOURCE_DIR}/itkOpenCVBasedBilateralImageFilter.h
    ${CMAKE_SOURCE_DIR}/itkOpenCVBasedBilateralImageFilter.hxx
)

add_executable(bf bf.cxx)

target_include_directories(bf PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${ITK_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(bf
    itkcvbf
    ${ITK_LIBRARIES}
    ${OpenCV_LIBS}
)
