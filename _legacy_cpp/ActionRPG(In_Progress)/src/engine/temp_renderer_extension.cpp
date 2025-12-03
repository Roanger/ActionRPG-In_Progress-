#include "renderer.h"
#include <iostream>

std::vector<Vertex> Renderer::generateGameWorldVertices(const std::shared_ptr<Level>& level, const std::shared_ptr<Character>& player, const VisualEffectManager& effectManager) {
    // Check if level and player are valid
    if (!level || !player) {
        std::cerr << "Error: Level or player is null in Renderer::generateGameWorldVertices" << std::endl;
        return {};
    }
    
    // Generate level vertices with frustum culling (only render what's visible)
    std::vector<Vertex> levelVertices = generateLevelVertices(level);
    
    // Generate player vertices (always visible at center)
    std::vector<Vertex> playerVertices = generateCharacterVertices(player);
    
    // Get enemies and only generate vertices for visible ones
    const auto& enemies = level->getEnemies();
    std::vector<Vertex> enemyVertices = generateEnemyVertices(enemies);
    
    // Generate UI elements if enabled
    std::vector<Vertex> uiVertices;
    if (showUI) {
        // Generate enemy health bars
        auto enemyHealthBars = uiSystem.generateEnemyHealthBars(enemies, cameraX, cameraY);
        
        // Generate player status bar
        auto playerStatusBar = uiSystem.generatePlayerStatusBar(player);
        
        // Combine UI vertices
        uiVertices.reserve(enemyHealthBars.size() + playerStatusBar.size());
        uiVertices.insert(uiVertices.end(), enemyHealthBars.begin(), enemyHealthBars.end());
        uiVertices.insert(uiVertices.end(), playerStatusBar.begin(), playerStatusBar.end());
    }
    
    // Get visual effect vertices
    std::vector<Vertex> effectVertices = effectManager.generateAllEffectVertices();
    
    // Get item drop vertices
    std::vector<Vertex> itemVertices = level->getItemDropManager().generateAllItemVertices();
    
    // Combine all vertices efficiently with reserve to avoid reallocations
    std::vector<Vertex> allVertices;
    allVertices.reserve(levelVertices.size() + playerVertices.size() + enemyVertices.size() + 
                      effectVertices.size() + itemVertices.size() + uiVertices.size());
    allVertices.insert(allVertices.end(), levelVertices.begin(), levelVertices.end());
    allVertices.insert(allVertices.end(), playerVertices.begin(), playerVertices.end());
    allVertices.insert(allVertices.end(), enemyVertices.begin(), enemyVertices.end());
    allVertices.insert(allVertices.end(), itemVertices.begin(), itemVertices.end());
    allVertices.insert(allVertices.end(), effectVertices.begin(), effectVertices.end());
    allVertices.insert(allVertices.end(), uiVertices.begin(), uiVertices.end());
    
    return allVertices;
}
