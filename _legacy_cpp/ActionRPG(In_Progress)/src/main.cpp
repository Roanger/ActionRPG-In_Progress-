#include "engine/vulkan_renderer.h"
#include "engine/game_loop.h"
#include <iostream>
#include <filesystem>
#include <string>

int main() {
    try {
        // Print the current working directory
        std::cout << "Current working directory: " << std::filesystem::current_path().string() << std::endl;

        // Initialize the renderer
        VulkanRenderer renderer;
        if (!renderer.initialize()) {
            std::cerr << "Failed to initialize Vulkan renderer" << std::endl;
            return -1;
        }

        // Initialize game loop
        GameLoop gameLoop(&renderer);
        
        // Run the game loop (which will create the surface first)
        gameLoop.run();

        // Cleanup
        renderer.cleanup();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
} 