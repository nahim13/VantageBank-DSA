cmake_minimum_required(VERSION 3.10)
project(VantageBank)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(CORE_SOURCES bank.cpp)

# Console Version
add_executable(bank_console main.cpp ${CORE_SOURCES})

# GUI Version
option(BUILD_GUI "Build with wxWidgets" ON)
if(BUILD_GUI)
    find_package(wxWidgets REQUIRED COMPONENTS core base)
    include(${wxWidgets_USE_FILE})
    add_definitions(-DUSE_WX)
    file(GLOB GUI_SOURCES "gui/*.cpp")
    add_executable(bank_gui ${GUI_SOURCES} ${CORE_SOURCES})
    target_include_directories(bank_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/gui)
    target_link_libraries(bank_gui ${wxWidgets_LIBRARIES})
endif()