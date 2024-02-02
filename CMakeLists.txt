cmake_minimum_required(VERSION 3.5)

# Setting the project name
project(Hybrid_Automata)

# Add the executable, specifying the source file(s).
add_executable(main src/main.cpp src/Automata.cpp src/Transition.cpp src/Node.cpp src/ScrapingJson.cpp src/tinyexpr.c src/System.cpp src/tools.cpp)

# Set compiler options based on the operating system
if(WIN32)
    set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/cl.exe")
    set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/cl.exe")
    target_compile_definitions(main PRIVATE WINDOWS)
endif()

# Set include directories
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Set compiler definitions
# Uncomment the line below if needed
# target_compile_definitions(main PRIVATE $<$<BOOL:${DEBUG_MODE}>:DEBUG_MODE>)
