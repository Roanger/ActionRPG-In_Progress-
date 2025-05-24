#include "renderer.h"
#include <iostream>

Renderer::Renderer(VulkanRenderer* vulkanRenderer) : 
    vulkanRenderer(vulkanRenderer),
    vertexBuffer(VK_NULL_HANDLE),
    vertexBufferMemory(VK_NULL_HANDLE) {
}

Renderer::~Renderer() {
    // Clean up vertex buffer if it exists and if vulkanRenderer is valid
    cleanupResources();
}

void Renderer::cleanupResources() {
    // Clean up vertex buffer if it exists and if vulkanRenderer is valid
    if (vulkanRenderer && vertexBuffer != VK_NULL_HANDLE) {
        VkDevice device = vulkanRenderer->getDevice();
        if (device != VK_NULL_HANDLE) {
            // Wait for the device to be idle before destroying resources
            vulkanRenderer->waitForDeviceIdle();
            
            // Clean up vertex buffer
            vkDestroyBuffer(device, vertexBuffer, nullptr);
            vertexBuffer = VK_NULL_HANDLE;
            
            // Clean up vertex buffer memory
            if (vertexBufferMemory != VK_NULL_HANDLE) {
                vkFreeMemory(device, vertexBufferMemory, nullptr);
                vertexBufferMemory = VK_NULL_HANDLE;
            }
        }
    }
}

void Renderer::initialize() {
    // Create initial empty vertex buffer
    std::vector<Vertex> emptyVertices;
    createVertexBuffer(emptyVertices);
}

void Renderer::render(const std::shared_ptr<Level>& level, const std::shared_ptr<Character>& player, const VisualEffectManager& effectManager) {
    // Performance optimization: Only log debug info occasionally
    static int frameCount = 0;
    bool debugFrame = (frameCount % 60 == 0);
    frameCount++;

    // Generate vertices for all game elements
    std::vector<Vertex> allVertices;

    // Level vertices
    std::vector<Vertex> levelVertices = generateLevelVertices(level);
    allVertices.insert(allVertices.end(), levelVertices.begin(), levelVertices.end());

    // Player vertices
    std::vector<Vertex> playerVertices = generateCharacterVertices(player);
    allVertices.insert(allVertices.end(), playerVertices.begin(), playerVertices.end());

    // Enemy vertices
    std::vector<Vertex> enemyVertices = generateEnemyVertices(level->getEnemies());
    allVertices.insert(allVertices.end(), enemyVertices.begin(), enemyVertices.end());

    // Item vertices
    std::vector<Vertex> itemVertices = generateItemVertices(level);
    allVertices.insert(allVertices.end(), itemVertices.begin(), itemVertices.end());

    // Visual effect vertices
    std::vector<Vertex> effectVertices = generateVisualEffectVertices(effectManager);
    allVertices.insert(allVertices.end(), effectVertices.begin(), effectVertices.end());

    // UI vertices
    std::vector<Vertex> uiVertices = uiSystem.generateUIVertices(player);
    allVertices.insert(allVertices.end(), uiVertices.begin(), uiVertices.end());

    // Update vertex buffer
    updateVertexBuffer(allVertices);

    // Debug output (optional)
    // std::cout << "Rendering: "
    //           << levelVertices.size() << " level, "
    //           << playerVertices.size() << " player, "
    //           << enemyVertices.size() << " enemies, "
    //           << itemVertices.size() << " items, "
    //           << effectVertices.size() << " effects, "
    //           << uiVertices.size() << " UI vertices." << std::endl;

    drawScene();
}

void Renderer::setCameraPosition(float x, float y) {
    cameraX = x;
    cameraY = y;
}

void Renderer::createVertexBuffer(const std::vector<Vertex>& vertices) {
    // Handle empty vertex arrays
    if (vertices.empty()) {
        std::cout << "Creating empty vertex buffer" << std::endl;
        // Create a minimal buffer for empty vertex arrays
        VkDeviceSize bufferSize = sizeof(Vertex);
        
        vulkanRenderer->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
        return;
    }
    
    // Calculate buffer size for non-empty vertex arrays
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    // Create a staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    vulkanRenderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                 stagingBuffer, stagingBufferMemory);
    
    // Map memory and copy vertices
    void* data;
    vkMapMemory(vulkanRenderer->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(vulkanRenderer->getDevice(), stagingBufferMemory);
    
    // Create the vertex buffer
    vulkanRenderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
    
    // Copy from staging buffer to vertex buffer
    vulkanRenderer->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
    
    // Clean up staging buffer
    vkDestroyBuffer(vulkanRenderer->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(vulkanRenderer->getDevice(), stagingBufferMemory, nullptr);
    
    //std::cout << "Vertex buffer created with " << vertices.size() << " vertices" << std::endl;

    // After creating the buffer, tell VulkanRenderer to use it.
    if (vulkanRenderer) {
        vulkanRenderer->setCurrentVertexBuffer(vertexBuffer, static_cast<uint32_t>(vertices.size()));
    }
}

void Renderer::updateVertexBuffer(const std::vector<Vertex>& vertices) {
    // For simplicity, we'll just recreate the vertex buffer
    if (vertexBuffer != VK_NULL_HANDLE) {
        // Wait for the device to be idle before destroying resources
        vulkanRenderer->waitForDeviceIdle();
        
        vkDestroyBuffer(vulkanRenderer->getDevice(), vertexBuffer, nullptr);
        vkFreeMemory(vulkanRenderer->getDevice(), vertexBufferMemory, nullptr);
    }
    
    createVertexBuffer(vertices);
}

void Renderer::drawScene() {
    // Performance optimization: Only log debug info occasionally
    static int frameCount = 0;
    bool debugFrame = (frameCount % 60 == 0);
    frameCount++;

    // For now, let's just generate some dummy vertices to test the rendering pipeline.
    // This will be replaced with actual game state later.
    std::vector<Vertex> allVertices;
    allVertices.push_back({{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
    allVertices.push_back({{0.5f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}});
    allVertices.push_back({{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}});

    updateVertexBuffer(allVertices);
    currentVertexCount = static_cast<uint32_t>(allVertices.size());
}

std::vector<Vertex> Renderer::generateLevelVertices(const std::shared_ptr<Level>& level) {
    std::vector<Vertex> vertices;
    
    // Check if level is valid
    if (!level) {
        return vertices;
    }
    
    // Calculate visible area around camera (frustum culling)
    // Only render tiles that are within view distance of the camera
    const int VIEW_DISTANCE = 15; // Adjust based on performance needs
    
    // Calculate the visible range based on camera position
    int startX = std::max(0, static_cast<int>(cameraX) - VIEW_DISTANCE);
    int endX = std::min(level->getWidth() - 1, static_cast<int>(cameraX) + VIEW_DISTANCE);
    int startY = std::max(0, static_cast<int>(cameraY) - VIEW_DISTANCE);
    int endY = std::min(level->getHeight() - 1, static_cast<int>(cameraY) + VIEW_DISTANCE);
    
    // Pre-allocate memory for vertices (optimization)
    vertices.reserve((endX - startX + 1) * (endY - startY + 1) * 6); // 6 vertices per tile (2 triangles)
    
    // Generate vertices only for tiles in the visible area
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            Tile tile = level->getTile(x, y);
            
            // Skip non-visible tiles
            if (!tile.visible && !tile.explored) continue;
            
            // Determine color based on tile type
            float r = 0.0f, g = 0.0f, b = 0.0f;
            
            switch (tile.type) {
                case TileType::FLOOR:
                    r = 0.5f; g = 0.5f; b = 0.5f; // Gray
                    break;
                case TileType::WALL:
                    r = 0.3f; g = 0.3f; b = 0.3f; // Dark gray
                    break;
                case TileType::DOOR:
                    r = 0.6f; g = 0.4f; b = 0.2f; // Brown
                    break;
                case TileType::WATER:
                    r = 0.0f; g = 0.0f; b = 0.8f; // Blue
                    break;
                case TileType::LAVA:
                    r = 0.8f; g = 0.2f; b = 0.0f; // Red
                    break;
            }
            
            // Make explored but not visible tiles darker
            if (!tile.visible && tile.explored) {
                r *= 0.5f;
                g *= 0.5f;
                b *= 0.5f;
            }
            
            // Create a square for the tile
            float tileSize = 0.02f; // Size of tile in normalized device coordinates
            
            // Use a fixed scale to make tiles a reasonable size
            float tileScale = 0.05f;
            
            // Calculate position with camera offset
            float xOffset = (x - cameraX) * tileScale * 2.0f;
            float yOffset = (y - cameraY) * tileScale * 2.0f;
            
            float xPos = xOffset;
            float yPos = yOffset;
            tileSize = tileScale;
            
            // Add vertices for the tile (two triangles forming a square)
            Vertex v1 = {{xPos, yPos}, {r, g, b}};
            Vertex v2 = {{xPos + tileSize * 2.0f, yPos}, {r, g, b}};
            Vertex v3 = {{xPos, yPos + tileSize * 2.0f}, {r, g, b}};
            Vertex v4 = {{xPos + tileSize * 2.0f, yPos + tileSize * 2.0f}, {r, g, b}};
            
            // First triangle
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
            // Second triangle
            vertices.push_back(v2);
            vertices.push_back(v4);
            vertices.push_back(v3);
        }
    }
    
    return vertices;
}

std::vector<Vertex> Renderer::generateCharacterVertices(const std::shared_ptr<Character>& character) {
    std::vector<Vertex> vertices;
    
    // Check if character is valid
    if (!character) {
        std::cout << "Warning: Null character pointer encountered" << std::endl;
        return vertices;
    }
    
    // Use a fixed scale to make the player a reasonable size
    float tileScale = 0.05f;
    
    // Player is always at the center of the screen
    float xPos = 0.0f;
    float yPos = 0.0f;
    float tileSize = tileScale;
    
    // Player is white
    float r = 1.0f, g = 1.0f, b = 1.0f;
    
    // Create a square for the player
    Vertex v1 = {{xPos, yPos}, {r, g, b}};
    Vertex v2 = {{xPos + tileSize * 2.0f, yPos}, {r, g, b}};
    Vertex v3 = {{xPos, yPos + tileSize * 2.0f}, {r, g, b}};
    Vertex v4 = {{xPos + tileSize * 2.0f, yPos + tileSize * 2.0f}, {r, g, b}};
    
    // First triangle
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    
    // Second triangle
    vertices.push_back(v2);
    vertices.push_back(v4);
    vertices.push_back(v3);
    
    return vertices;
}

std::vector<Vertex> Renderer::generateEnemyVertices(const std::vector<std::shared_ptr<Enemy>>& enemies) {
    std::vector<Vertex> vertices;
    
    // Check if enemies vector is empty
    if (enemies.empty()) {
        return vertices;
    }
    
    // Define view distance for frustum culling
    const float VIEW_DISTANCE = 15.0f;
    
    // Pre-allocate memory for vertices (optimization)
    vertices.reserve(enemies.size() * 6); // 6 vertices per enemy (2 triangles)
    
    // Generate vertices only for enemies within view distance
    for (const auto& enemy : enemies) {
        // Skip invalid enemies
        if (!enemy) continue;
        
        // Calculate distance from camera/player
        float dx = enemy->getX() - cameraX;
        float dy = enemy->getY() - cameraY;
        float distanceSquared = dx * dx + dy * dy;
        
        // Skip enemies outside view distance (frustum culling)
        if (distanceSquared > VIEW_DISTANCE * VIEW_DISTANCE) continue;
        
        // Use a fixed scale to make enemies a reasonable size
        float tileScale = 0.05f;
        
        // Apply camera offset to position enemies relative to the player
        float xOffset = dx * tileScale * 2.0f;
        float yOffset = dy * tileScale * 2.0f;
        
        float xPos = xOffset;
        float yPos = yOffset;
        float tileSize = tileScale;
        
        // Determine color based on enemy type
        float r = 0.0f, g = 0.0f, b = 0.0f;
        
        switch (enemy->getEnemyType()) {
            case EnemyType::GOBLIN:
                r = 0.0f; g = 0.8f; b = 0.0f; // Green
                break;
            case EnemyType::SKELETON:
                r = 0.8f; g = 0.8f; b = 0.8f; // Light gray
                break;
            case EnemyType::ORC:
                r = 0.0f; g = 0.5f; b = 0.0f; // Dark green
                break;
            case EnemyType::TROLL:
                r = 0.5f; g = 0.5f; b = 0.0f; // Olive
                break;
            case EnemyType::DRAGON:
                r = 1.0f; g = 0.0f; b = 0.0f; // Red
                break;
        }
        
        // Create a square for the enemy
        Vertex v1 = {{xPos, yPos}, {r, g, b}};
        Vertex v2 = {{xPos + tileSize * 2.0f, yPos}, {r, g, b}};
        Vertex v3 = {{xPos, yPos + tileSize * 2.0f}, {r, g, b}};
        Vertex v4 = {{xPos + tileSize * 2.0f, yPos + tileSize * 2.0f}, {r, g, b}};
        
        // First triangle
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        
        // Second triangle
        vertices.push_back(v2);
        vertices.push_back(v4);
        vertices.push_back(v3);
    }
    
    return vertices;
}

std::vector<Vertex> Renderer::generateItemVertices(const std::shared_ptr<Level>& level) {
    std::vector<Vertex> vertices;
    const float TILE_SIZE = 0.1f; // Size of an item on the screen

    for (const auto& item : level->getItems()) {
        if (!item) continue;

        float x = item->getX() * TILE_SIZE;
        float y = item->getY() * TILE_SIZE;

        // Define vertices for the item (e.g., a simple square)
        // Color can be based on item type or a default color
        float r = 1.0f, g = 1.0f, b = 0.0f; // Yellow for items

        vertices.push_back({{x, y}, {r, g, b}});
        vertices.push_back({{x + TILE_SIZE, y}, {r, g, b}});
        vertices.push_back({{x, y + TILE_SIZE}, {r, g, b}});

        vertices.push_back({{x + TILE_SIZE, y}, {r, g, b}});
        vertices.push_back({{x + TILE_SIZE, y + TILE_SIZE}, {r, g, b}});
        vertices.push_back({{x, y + TILE_SIZE}, {r, g, b}});
    }
    return vertices;
}

std::vector<Vertex> Renderer::generateVisualEffectVertices(const VisualEffectManager& effectManager) {
    std::vector<Vertex> vertices;
    for (const auto& effect : effectManager.getEffects()) {
        if (effect) {
            auto effectVertices = effect->generateVertices();
            vertices.insert(vertices.end(), effectVertices.begin(), effectVertices.end());
        }
    }
    return vertices;
}

std::vector<Vertex> Renderer::generateUIVertices(const std::shared_ptr<Character>& player) {
    // This function will delegate to the UISystem to generate UI vertices
    // The UISystem will handle the actual UI element generation
    return uiSystem.generateUIVertices(player);
}