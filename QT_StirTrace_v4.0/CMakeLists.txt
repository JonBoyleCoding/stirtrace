cmake_minimum_required(VERSION 3.29)

project(StirTrace)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set Qt auto stuff
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(Qt6Widgets REQUIRED)
find_package(OpenCV 2 REQUIRED)

# Include OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Export compile commands for editor
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# Add project files
set(SOURCES ddrawreader.cpp ddrawwriter.cpp evaluation.cpp filehandler.cpp imgproc.cpp main.cpp mainwindow.cpp preprocessing.cpp)
set(HEADERS IBinaryFileReader.h IBinaryFileWriter.h ddrawreader.h ddrawwriter.h evaluation.h filehandler.h imgproc.h mainwindow.h preprocessing.h)
set(UI mainwindow.ui)

# Setup compiler / linker
add_executable(stirtrace ${SOURCES} ${HEADERS} ${UI})
target_link_libraries(stirtrace Qt6::Widgets ${OpenCV_LIBS})
