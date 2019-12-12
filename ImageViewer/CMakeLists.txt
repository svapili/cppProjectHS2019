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

# openCV and Tesseract
set(OpenCV_DIR "~/opencv/build")
find_package(OpenCV REQUIRED)
find_package( Tesseract 4.0 REQUIRED )

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5PrintSupport REQUIRED)      # required by QCustomPlot
find_package(qtlibs)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Populate CMake variable with the sources
set(APP_SOURCES
    src/MainWindow.cpp
    src/MainWindow.h
    src/CaptureScreen.cpp
    src/CaptureScreen.h
)

# including all cpp/h files in the current directory
aux_source_directory(. SRC_LIST)

# Create executable using the specified src
add_executable(${PROJECT_NAME} src/main.cpp ${APP_SOURCES})

INCLUDE_DIRECTORIES(${APP_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS}
    ${Tesseract_INCLUDE_DIRS} ${LEPTONICA_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/ui
    )

# Additional Library Directories
link_directories( ${OpenCV_LIB_DIR} ${TESSERACT_LIBRARY_DIRS} ${LEPTONICA_LIBRARY_DIRS} )

# link required libs
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets
    Qt5::PrintSupport ${OpenCV_LIBS} ${Tesseract_LIBRARIES} ${LEPTONICA_LIBRARIES})

# copy pretrained model data from openCV EAST classifier
file(COPY src/frozen_east_text_detection.pb DESTINATION ${PROJECT_BINARY_DIR})

# Set compile options, enable warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall>
    $<$<CXX_COMPILER_ID:MSVC>: /Wall>
)
