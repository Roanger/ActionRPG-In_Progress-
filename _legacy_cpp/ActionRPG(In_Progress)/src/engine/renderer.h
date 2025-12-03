#pragma once

#include <vector>
#include <memory>
#include "vertex.h"
#include "vulkan_renderer.h"
#include "../game/level.h"
#include "../game/character.h"
#include "../game/enemy.h"
#include "../game/visual_effect.h"
#include "../ui/ui_system.h"

// Using the common vertex structure from vertex.h
// (already included via vulkan_renderer.h)

class Renderer {
public:
    Renderer(VulkanRenderer* vulkanRenderer);
    ~Renderer();
    
    void initialize();
    void render(const std::shared_ptr<Level>& level, const std::shared_ptr<Character>& player, const VisualEffectManager& effectManager);
    void cleanupResources();
    
    void drawScene(); // New function to be called by VulkanRenderer
    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    uint32_t getVertexCount() const { return currentVertexCount; }
    
    // Toggle UI visibility
    void setShowUI(bool show) { showUI = show; } // Always show UI for now
    
    void setCameraPosition(float x, float y);
    
    void updateVertexBuffer(const std::vector<Vertex>& vertices);
    
    // Generate all vertices for the game world (level, player, enemies, effects)
    std::vector<Vertex> generateGameWorldVertices(const std::shared_ptr<Level>& level, const std::shared_ptr<Character>& player, const VisualEffectManager& effectManager);
    
private:
    VulkanRenderer* vulkanRenderer;
    
    // Vertex buffer
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    
    // Camera variables
    float cameraX = 0.0f;
    float cameraY = 0.0f;
    
    // UI system
    UISystem uiSystem;
    bool showUI = true;
    
    uint32_t currentVertexCount = 0; // Current number of vertices

    // Methods
    void createVertexBuffer();
    std::vector<Vertex> generateLevelVertices(const std::shared_ptr<Level>& level);
    std::vector<Vertex> generateCharacterVertices(const std::shared_ptr<Character>& character);
    std::vector<Vertex> generateEnemyVertices(const std::vector<std::shared_ptr<Enemy>>& enemies);
    std::vector<Vertex> generateItemVertices(const std::shared_ptr<Level>& level);
    std::vector<Vertex> generateVisualEffectVertices(const VisualEffectManager& effectManager);
    std::vector<Vertex> generateUIVertices(const std::shared_ptr<Character>& player);

    // Helper methods for level generation
    void createRoom(int x1, int y1, int x2, int y2);
    void createCorridor(int x1, int y1, int x2, int y2);
    
    // Enemy drop system
    void dropLoot(float x, float y, int enemyLevel);
}; 