# Set Name of project and language
project(ImageViewer LANGUAGES CXX)

# Set cmake version
cmake_minimum_required(VERSION 3.0)

# activate latest c++ compiler version
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# set flags to configure the warning settings
# Note: warnings are compiler specific
if( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# set build type to Debug/Release
set(CMAKE_BUILD_TYPE "Debug")

find_package(qtlibs)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5PrintSupport REQUIRED)      # required by QCustomPlot

# Populate a CMake variable with the sources
set(APP_SOURCES
    src/MainWindow.cpp
    src/MainWindow.h
    #src/ui/widget.ui
    #src/ui/widget.cpp
    #src/ui/widget.h
    #src/ui/qcustomplot-source/qcustomplot.cpp
    #src/ui/qcustomplot-source/qcustomplot.h
)

# including all cpp/h files in the current directory
aux_source_directory(. SRC_LIST)

# Create executable using the specified src
add_executable(${PROJECT_NAME} src/main.cpp ${APP_SOURCES})

set(APP_INCLUDE_DIRS
    src/
    src/ui/
    #src/ui/qcustomplot-source
)
INCLUDE_DIRECTORIES(${APP_INCLUDE_DIRS})

# Use the Widgets module from Qt 5
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::PrintSupport)

# copy textfile
#file(COPY ekg.txt DESTINATION ${PROJECT_BINARY_DIR})


# Set compile options, enable warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall>
    $<$<CXX_COMPILER_ID:MSVC>: /Wall>
)
