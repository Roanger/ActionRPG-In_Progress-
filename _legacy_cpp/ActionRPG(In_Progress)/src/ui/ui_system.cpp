#include "ui_system.h"
#include <iostream>
#include <fstream>
#include <vector>

// UISystem::UISystem() {
//     // Load font and initialize Trex objects
//     // Assuming a font file named 'arial.ttf' exists in a 'fonts' directory relative to the executable
//     std::string fontPath = "fonts/arial.ttf";
//     int fontSize = 24; // Default font size
    
//     // Read font file into a buffer
//     std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
//     if (!fontFile.is_open()) {
//         std::cerr << "Error: Could not open font file: " << fontPath << std::endl;
//         // Handle error: perhaps load a fallback font or disable text rendering
//         return;
//     }
    
//     size_t fileSize = fontFile.tellg();
//     std::vector<uint8_t> fontBuffer(fileSize);
//     fontFile.seekg(0);
//     fontFile.read(reinterpret_cast<char*>(fontBuffer.data()), fileSize);
//     fontFile.close();
    
//     // Create font atlas and text shaper
//     try {
//         fontAtlas = std::make_unique<Trex::Atlas>(fontBuffer.data(), fileSize, fontSize, Trex::Charset::Ascii());
//         textShaper = std::make_unique<Trex::TextShaper>(*fontAtlas);
//         std::cout << "Trex font atlas and text shaper initialized successfully." << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Error initializing Trex: " << e.what() << std::endl;
//         // Handle error
//     }
// }

UISystem::~UISystem() {
}

std::vector<Vertex> UISystem::generateHealthBarVertices(const std::shared_ptr<Character>& character, float xOffset, float yOffset) {
    if (!character) {
        return {};
    }
    
    // Calculate health percentage
    float healthPercent = static_cast<float>(character->getHealth()) / static_cast<float>(character->getMaxHealth());
    
    // Create health bar
    return createHealthBar(xOffset, yOffset, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, healthPercent);
}

std::vector<Vertex> UISystem::generateEnemyHealthBars(const std::vector<std::shared_ptr<Enemy>>& enemies, float cameraX, float cameraY) {
    std::vector<Vertex> vertices;
    
    // Only show health bars for enemies within view distance
    const float VIEW_DISTANCE = 15.0f;
    const float TILE_SCALE = 0.05f;
    
    for (const auto& enemy : enemies) {
        if (!enemy) continue;
        
        // Calculate distance from camera
        float dx = enemy->getX() - cameraX;
        float dy = enemy->getY() - cameraY;
        float distanceSquared = dx * dx + dy * dy;
        
        // Skip enemies outside view distance
        if (distanceSquared > VIEW_DISTANCE * VIEW_DISTANCE) continue;
        
        // Calculate position relative to camera
        // Shift health bar to the right a bit
        float xOffset = dx * TILE_SCALE * 2.0f - (HEALTH_BAR_WIDTH / 2.0f) + 0.05f; // Center above enemy with slight offset
        float yOffset = dy * TILE_SCALE * 2.0f - ENEMY_HEALTH_BAR_OFFSET_Y; // Above the enemy
        
        // Always show health bars for enemies
        float healthPercent = static_cast<float>(enemy->getHealth()) / static_cast<float>(enemy->getMaxHealth());
        auto healthBar = createHealthBar(xOffset, yOffset, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, healthPercent);
        vertices.insert(vertices.end(), healthBar.begin(), healthBar.end());
    }
    
    return vertices;
}

std::vector<Vertex> UISystem::generatePlayerStatusBar(const std::shared_ptr<Character>& player) {
    std::vector<Vertex> vertices;
    
    if (!player) {
        return vertices;
    }
    
    // Create player health bar directly above the player
    float healthPercent = static_cast<float>(player->getHealth()) / static_cast<float>(player->getMaxHealth());
    
    // Position the health bar centered above the player character
    // Since player is always at center (0,0), we just need to offset vertically
    // Add a slight horizontal offset to match enemy health bars
    float xOffset = -HEALTH_BAR_WIDTH / 2.0f + 0.05f; // Shifted right a bit
    float yOffset = -0.12f; // Just above the player
    
    auto healthBar = createHealthBar(xOffset, yOffset, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, healthPercent);
    vertices.insert(vertices.end(), healthBar.begin(), healthBar.end());
    
    // Add player stats text (level, class, etc.)
    // In a real implementation, we would need text rendering support
    // For now, we'll just add placeholders for the status area
    
    return vertices;
}

std::vector<Vertex> UISystem::createHealthBar(float x, float y, float width, float height, float healthPercent) {
    std::vector<Vertex> vertices;
    
    // Clamp health percentage between 0 and 1
    healthPercent = std::max(0.0f, std::min(1.0f, healthPercent));
    
    // Border color (black)
    float borderR = 0.0f, borderG = 0.0f, borderB = 0.0f;
    
    // Background (dark gray)
    float bgR = 0.2f, bgG = 0.2f, bgB = 0.2f;
    
    // Health bar colors (green to red based on health)
    float r = 1.0f - healthPercent;
    float g = healthPercent;
    float b = 0.0f;
    
    // Add border around health bar (black rectangle)
    float borderX = x - HEALTH_BAR_BORDER;
    float borderY = y - HEALTH_BAR_BORDER;
    float borderWidth = width + (HEALTH_BAR_BORDER * 2.0f);
    float borderHeight = height + (HEALTH_BAR_BORDER * 2.0f);
    
    // Border vertices
    Vertex borderV1 = {{borderX, borderY}, {borderR, borderG, borderB}};
    Vertex borderV2 = {{borderX + borderWidth, borderY}, {borderR, borderG, borderB}};
    Vertex borderV3 = {{borderX, borderY + borderHeight}, {borderR, borderG, borderB}};
    Vertex borderV4 = {{borderX + borderWidth, borderY + borderHeight}, {borderR, borderG, borderB}};
    
    // First triangle (border)
    vertices.push_back(borderV1);
    vertices.push_back(borderV2);
    vertices.push_back(borderV3);
    
    // Second triangle (border)
    vertices.push_back(borderV2);
    vertices.push_back(borderV4);
    vertices.push_back(borderV3);
    
    // Background vertices (gray bar)
    Vertex bgV1 = {{x, y}, {bgR, bgG, bgB}};
    Vertex bgV2 = {{x + width, y}, {bgR, bgG, bgB}};
    Vertex bgV3 = {{x, y + height}, {bgR, bgG, bgB}};
    Vertex bgV4 = {{x + width, y + height}, {bgR, bgG, bgB}};
    
    // First triangle (background)
    vertices.push_back(bgV1);
    vertices.push_back(bgV2);
    vertices.push_back(bgV3);
    
    // Second triangle (background)
    vertices.push_back(bgV2);
    vertices.push_back(bgV4);
    vertices.push_back(bgV3);
    
    // Health bar vertices (colored based on health)
    float healthWidth = width * healthPercent;
    
    if (healthWidth > 0.001f) { // Only draw health bar if there's health to show
        Vertex hbV1 = {{x, y}, {r, g, b}};
        Vertex hbV2 = {{x + healthWidth, y}, {r, g, b}};
        Vertex hbV3 = {{x, y + height}, {r, g, b}};
        Vertex hbV4 = {{x + healthWidth, y + height}, {r, g, b}};
        
        // First triangle (health)
        vertices.push_back(hbV1);
        vertices.push_back(hbV2);
        vertices.push_back(hbV3);
        
        // Second triangle (health)
        vertices.push_back(hbV2);
        vertices.push_back(hbV4);
        vertices.push_back(hbV3);
    }
    
    return vertices;
}

std::vector<Vertex> UISystem::generateUIVertices(const std::shared_ptr<Character>& player) {
    std::vector<Vertex> uiVertices;

    // Generate player status bar vertices
    auto playerStatusBarVertices = generatePlayerStatusBar(player);
    uiVertices.insert(uiVertices.end(), playerStatusBarVertices.begin(), playerStatusBarVertices.end());

    // In the future, we might add enemy health bars here as well, 
    // but for now, we'll keep it simple.

    return uiVertices;
}

// std::vector<Vertex> UISystem::createStatusText(float x, float y, const std::string& text) {
//     std::vector<Vertex> vertices;
//     
//     if (!fontAtlas || !textShaper) {
//         std::cerr << "Error: Font atlas or text shaper not initialized." << std::endl;
//         return vertices; // Return empty if not initialized
//     }
//     
//     // Shape the text
//     Trex::ShapedGlyphs glyphs = textShaper->ShapeAscii(text); // Or ShapeUtf8 if needed
//     
//     // Generate vertices for each glyph
//     float currentX = x;
//     float currentY = y;
//     
//     for (const auto& glyph : glyphs) {
//         // Calculate glyph position
//         float glyphX = currentX + glyph.xOffset + glyph.info.bearingX;
//         float glyphY = currentY + glyph.yOffset - glyph.info.bearingY; // Trex Y is often inverted for rendering
//         
//         // Calculate glyph quad corners
//         float x0 = glyphX;
//         float y0 = glyphY;
//         float x1 = glyphX + glyph.info.width;
//         float y1 = glyphY + glyph.info.height;
//         
//         // Calculate texture coordinates (UVs)
//         float u0 = (float)glyph.info.x / fontAtlas->GetBitmap().Width();
//         float v0 = (float)glyph.info.y / fontAtlas->GetBitmap().Height();
//         float u1 = (float)(glyph.info.x + glyph.info.width) / fontAtlas->GetBitmap().Width();
//         float v1 = (float)(glyph.info.y + glyph.info.height) / fontAtlas->GetBitmap().Height();
//         
//         // Define default text color (white)
//         float r = 1.0f, g = 1.0f, b = 1.0f;
//         
//         // Create vertices for the two triangles forming the glyph quad
//         vertices.push_back({{x0, y0}, {r, g, b}, {u0, v0}});
//         vertices.push_back({{x1, y0}, {r, g, b}, {u1, v0}});
//         vertices.push_back({{x0, y1}, {r, g, b}, {u0, v1}});
// 
//         vertices.push_back({{x1, y0}, {r, g, b}, {u1, v0}});
//         vertices.push_back({{x1, y1}, {r, g, b}, {u1, v1}});
//         vertices.push_back({{x0, y1}, {r, g, b}, {u0, v1}});
//         
//         // Advance for the next glyph
//         currentX += glyph.xAdvance;
//         currentY += glyph.yAdvance; // Usually 0 for horizontal text
//     }
//     
//     return vertices;
// }
