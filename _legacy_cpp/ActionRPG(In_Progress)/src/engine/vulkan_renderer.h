#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <array>

// Include the common vertex structure
#include "vertex.h"

class Renderer; // Forward declaration

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    bool initialize();
    void cleanup();
    
    bool createSurface(GLFWwindow* window);
    bool render();
    bool initializeSwapchain();
    
    void setRenderer(Renderer* renderer) { this->renderer_ = renderer; }
    
    // Getters
    VkInstance getInstance() const { return instance; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkDevice getDevice() const { return device; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    
    void setCurrentVertexBuffer(VkBuffer buffer, uint32_t vertexCount) {
        currentVertexBuffer = buffer;
        currentVertexCount = vertexCount;
    }
    
    // Wait for the device to be idle before destroying resources
    void waitForDeviceIdle() {
        if (device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(device);
        }
    }

    VkCommandPool getCommandPool() const { return commandPool; }
    VkCommandBuffer getCommandBuffer(uint32_t imageIndex) const { return commandBuffers[imageIndex]; }
    std::array<VkDeviceSize, 1> getOffsets() const { return {0}; }
    
    // Buffer management functions
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    
private:
    // Vulkan components
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    
    Renderer* renderer_ = nullptr; // Pointer to the main Renderer instance

    // Swapchain components
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchainImages;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    std::vector<VkImageView> swapchainImageViews;
    
    // Pipeline components
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    
    // Command components
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    
    // Synchronization objects
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    
    // Framebuffers
    std::vector<VkFramebuffer> swapchainFramebuffers;
    
    // Synchronization
    const int MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
    
    // Record command buffer
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    
    // Helper functions
    bool createInstance();
    bool setupDebugMessenger();
    bool pickPhysicalDevice();
    bool createLogicalDevice();
    bool createSwapChain();
    bool createImageViews();
    bool createRenderPass();
    bool createGraphicsPipeline();
    bool createFramebuffers();
    bool createCommandPool();
    bool createCommandBuffers();
    bool createSyncObjects();
    
    // Shader handling
    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    
    // Validation layers
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    
    // Device extensions
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    VkBuffer currentVertexBuffer = VK_NULL_HANDLE;
    uint32_t currentVertexCount = 0;
    
    // Store shader modules for cleanup
    std::vector<VkShaderModule> shaderModules;
}; 