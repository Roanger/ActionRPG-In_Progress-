cmake_minimum_required(VERSION 3.10)
message("Running CMake...")
project(ActionRPG VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Add Trex library using FetchContent
include(FetchContent)
# FetchContent_Declare(
#    trex
#    GIT_REPOSITORY https://github.com/KyrietS/trex.git
#    GIT_TAG        master # Use the master branch for the latest version
#)
#FetchContent_MakeAvailable(trex)

# Specify the exact path to GLFW
set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/glfw3.lib")

# Check if the files exist
if(EXISTS "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h" AND EXISTS "${GLFW_LIBRARY}")
    message(STATUS "Found GLFW: ${GLFW_LIBRARY}")
    # Add GLFW include directory
    include_directories(${GLFW_INCLUDE_DIR})
else()
    message(FATAL_ERROR "GLFW not found. Please make sure you have the following files:
    - ${GLFW_INCLUDE_DIR}/GLFW/glfw3.h
    - ${GLFW_LIBRARY}")
endif()

# Define source files
set(SOURCES
    src/main.cpp
    
    # Engine files
    src/engine/game_loop.cpp
    src/engine/renderer.cpp
    src/engine/vulkan_renderer.cpp
    
    # Game files
    src/game/character.cpp
    src/game/enemy.cpp
    src/game/item.cpp
    src/game/item_drop.cpp
    src/game/level.cpp
    src/game/visual_effect.cpp
    
    # UI files
    src/ui/ui_system.cpp
    src/ui/character_select.cpp
    src/ui/inventory_ui.cpp
)

# Define header files
set(HEADERS
    src/engine/game_loop.h
    src/engine/renderer.h
    src/engine/vulkan_renderer.h
    src/engine/vertex.h
    
    src/game/character.h
    src/game/enemy.h
    src/game/item.h
    src/game/item_drop.h
    src/game/level.h
    src/game/visual_effect.h
    
    # UI headers
    src/ui/ui_system.h
    src/ui/inventory_ui.h
    src/ui/character_select.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
#    ${trex_SOURCE_DIR}/include # Explicitly add Trex include directory
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    ${GLFW_LIBRARY}
#    trex # Link the Trex library
)

# Define compile options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy shader files if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    )
endif()

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Enable testing
enable_testing()

# Output information
message(STATUS "Vulkan include dirs: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan libraries: ${Vulkan_LIBRARIES}") 