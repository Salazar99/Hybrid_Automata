cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0084 OLD)


# Setting the project name
project(Hybrid_Automata)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)


# Add the executable, specifying the source file(s).
add_executable(main src/main.cpp src/Automata.cpp src/Transition.cpp src/Node.cpp src/ScrapingJson.cpp src/tinyexpr.c src/System.cpp src/tools.cpp
    src/mainwindow.cpp src/mainwindow.ui  include/mainwindow.h include/csvfile.h include/json.hpp include/Objects.h include/tinyexpr.h
    include/tools.h include/UtilsJson.h
    include/arrowitem.h
    src/arrowitem.cpp
    include/circleitem.h
    include/style.h
    include/switch.h
    src/switch.cpp



)

# Set compiler options based on the operating system
if(WIN32)
    #set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/cl.exe")
    #set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/cl.exe")
    target_compile_definitions(main PRIVATE WINDOWS)
endif()

# Set include directories
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(main PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Set compiler definitions
# Uncomment the line below if needed
# target_compile_definitions(main PRIVATE $<$<BOOL:${DEBUG_MODE}>:DEBUG_MODE>)
