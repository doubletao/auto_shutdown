cmake_minimum_required(VERSION 3.15)
project(auto_shutdown)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(auto_shutdown WIN32 main.cpp)

target_link_libraries(auto_shutdown PRIVATE
    Qt5::Widgets
    user32
    advapi32
)

if(MSVC)
    set_target_properties(auto_shutdown PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()
