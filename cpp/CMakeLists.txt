cmake_minimum_required(VERSION 3.10)
project(PowerMeterSCPI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find VISA package (NI-VISA)
find_package(VISA REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/VisaDevice.cpp
    src/PowerMeterSCPI.cpp
    src/DataLogger.cpp
)

set(HEADERS
    include/VisaDevice.h
    include/PowerMeterSCPI.h
    include/DataLogger.h
)

# Create executable
add_executable(PowerMeterApp ${SOURCES} ${HEADERS})

# Link VISA library
target_link_libraries(PowerMeterApp PRIVATE VISA::VISA)

# Platform specific settings
if(MSVC)
    # Visual Studio specific settings
    target_compile_options(PowerMeterApp PRIVATE /W4)
else()
    # GCC/Clang settings
    target_compile_options(PowerMeterApp PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Output directory
set_target_properties(PowerMeterApp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
