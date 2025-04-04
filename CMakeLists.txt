cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and C languages
enable_language(CXX C ASM)

# YRShell Defines
set(YRShell_Defines 

)

# YRShell generated include paths
set(YRShell_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/utility
)

# YRShell generated application sources
set(YRShell_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/YRShell.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/YRShellInterpreter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/DebugLog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/Dictionary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/HardwareSpecific.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/HiResTimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/IntervalTimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/Sliceable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/BufferedSerial.cpp
)

# Link directories setup
set(YRShell_LINK_DIRS

)
# Project static libraries
set(YRShell_LINK_LIBS 
    
)

# Interface library for includes and symbols
add_library(yrshell INTERFACE)
target_include_directories(yrshell INTERFACE ${YRShell_Include_Dirs})
target_compile_definitions(yrshell INTERFACE ${YRShell_Defines_Syms})

# Create YRShell static library
# add_library(STM32_Drivers OBJECT)
# target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
# target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)

# Add STM32CubeMX generated application sources to the project
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${YRShell_Application_Src})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${YRShell_LINK_DIRS})

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} ${YRShell_LINK_LIBS})

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()