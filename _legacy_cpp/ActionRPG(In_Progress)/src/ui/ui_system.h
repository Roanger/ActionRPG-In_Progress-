#pragma once

#include "../game/character.h"
#include "../game/enemy.h"
#include "../engine/vertex.h"
#include <memory>
#include <vector>
#include <string>

// #include "Trex/Atlas.h"
// #include "Trex/TextShaper.h"

class UISystem {
public:
    UISystem();
    ~UISystem();
    
    // Generate vertices for UI elements
    std::vector<Vertex> generateHealthBarVertices(const std::shared_ptr<Character>& character, float xOffset, float yOffset);
    std::vector<Vertex> generateEnemyHealthBars(const std::vector<std::shared_ptr<Enemy>>& enemies, float cameraX, float cameraY);
    std::vector<Vertex> generatePlayerStatusBar(const std::shared_ptr<Character>& player);
    std::vector<Vertex> generateUIVertices(const std::shared_ptr<Character>& player);
    
    // Helper methods
    std::vector<Vertex> createHealthBar(float x, float y, float width, float height, float healthPercent);
    std::vector<Vertex> createStatusText(float x, float y, const std::string& text);
    
private:
    // Constants for UI layout
    const float HEALTH_BAR_WIDTH = 0.12f;        // Slightly wider
    const float HEALTH_BAR_HEIGHT = 0.025f;      // Slightly taller
    const float HEALTH_BAR_BORDER = 0.003f;      // Border thickness
    const float ENEMY_HEALTH_BAR_OFFSET_Y = 0.15f; // Position above enemy
    const float PLAYER_STATUS_BAR_HEIGHT = 0.05f;
    
    // Trex text rendering members
//    std::unique_ptr<Trex::Atlas> fontAtlas;
//    std::unique_ptr<Trex::TextShaper> textShaper;
};
