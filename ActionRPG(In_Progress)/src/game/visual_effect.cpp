#include "visual_effect.h"
#include <cmath>
#include <iostream>

// Define static color constants
const float VisualEffect::SLASH_COLOR[3] = {0.9f, 0.9f, 0.1f};      // Yellow
const float VisualEffect::ARROW_COLOR[3] = {0.2f, 0.8f, 0.2f};      // Green
const float VisualEffect::FIREBALL_COLOR[3] = {0.9f, 0.4f, 0.1f};   // Orange-red

VisualEffect::VisualEffect(Character::VisualEffectType type, float startX, float startY, float endX, float endY)
    : type(type), startX(startX), startY(startY), endX(endX), endY(endY), scale(1.0f), rotation(0.0f) {
    
    // Set lifetime based on effect type
    switch (type) {
        case Character::VisualEffectType::SLASH:
            maxLifetime = 0.3f;  // Short duration for slash
            break;
        case Character::VisualEffectType::ARROW:
            maxLifetime = 0.5f;  // Medium duration for arrow
            break;
        case Character::VisualEffectType::FIREBALL:
            maxLifetime = 0.7f;  // Longer duration for fireball
            break;
        default:
            maxLifetime = 0.5f;
    }
    
    lifetime = maxLifetime;
}

VisualEffect::~VisualEffect() {
}

void VisualEffect::update(float deltaTime) {
    lifetime -= deltaTime;
    if (lifetime < 0.0f) {
        lifetime = 0.0f;
    }
    
    // Update effect properties based on lifetime
    float progress = 1.0f - (lifetime / maxLifetime);
    
    // Different effects have different animations
    switch (type) {
        case Character::VisualEffectType::SLASH:
            // Slash grows and fades
            scale = 1.0f + progress * 0.5f;
            rotation = progress * 3.14159f; // Rotate 180 degrees
            break;
            
        case Character::VisualEffectType::ARROW:
            // Arrow moves from start to end
            // No special animation needed as position is interpolated in generateVertices
            break;
            
        case Character::VisualEffectType::FIREBALL:
            // Fireball grows slightly and pulses
            scale = 1.0f + 0.2f * sin(progress * 10.0f);
            break;
            
        default:
            break;
    }
}

std::vector<Vertex> VisualEffect::generateVertices() const {
    switch (type) {
        case Character::VisualEffectType::SLASH:
            return generateSlashVertices();
        case Character::VisualEffectType::ARROW:
            return generateArrowVertices();
        case Character::VisualEffectType::FIREBALL:
            return generateFireballVertices();
        default:
            return std::vector<Vertex>();
    }
}

std::vector<Vertex> VisualEffect::generateSlashVertices() const {
    std::vector<Vertex> vertices;
    
    if (lifetime <= 0.0f) return vertices;
    
    // Calculate position (slash appears at the end position)
    float x = endX;
    float y = endY;
    
    // Calculate alpha based on lifetime
    float alpha = lifetime / maxLifetime;
    
    // Create a slash shape (diagonal line with thickness)
    float size = 0.15f * scale;
    float thickness = 0.03f * scale;
    
    // Apply rotation
    float cosR = cos(rotation);
    float sinR = sin(rotation);
    
    // Define the four corners of the slash
    float x1 = -size, y1 = -size;
    float x2 = size, y2 = -size;
    float x3 = -size + thickness, y3 = size;
    float x4 = size + thickness, y4 = size;
    
    // Rotate points
    float rx1 = x1 * cosR - y1 * sinR;
    float ry1 = x1 * sinR + y1 * cosR;
    float rx2 = x2 * cosR - y2 * sinR;
    float ry2 = x2 * sinR + y2 * cosR;
    float rx3 = x3 * cosR - y3 * sinR;
    float ry3 = x3 * sinR + y3 * cosR;
    float rx4 = x4 * cosR - y4 * sinR;
    float ry4 = x4 * sinR + y4 * cosR;
    
    // Create vertices with color
    Vertex v1 = {{x + rx1, y + ry1}, {SLASH_COLOR[0], SLASH_COLOR[1], SLASH_COLOR[2]}};
    Vertex v2 = {{x + rx2, y + ry2}, {SLASH_COLOR[0], SLASH_COLOR[1], SLASH_COLOR[2]}};
    Vertex v3 = {{x + rx3, y + ry3}, {SLASH_COLOR[0], SLASH_COLOR[1], SLASH_COLOR[2]}};
    Vertex v4 = {{x + rx4, y + ry4}, {SLASH_COLOR[0], SLASH_COLOR[1], SLASH_COLOR[2]}};
    
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

std::vector<Vertex> VisualEffect::generateArrowVertices() const {
    std::vector<Vertex> vertices;
    
    if (lifetime <= 0.0f) return vertices;
    
    // Calculate position (interpolate from start to end)
    float progress = 1.0f - (lifetime / maxLifetime);
    float x = startX + (endX - startX) * progress;
    float y = startY + (endY - startY) * progress;
    
    // Calculate direction for arrow orientation
    float dx = endX - startX;
    float dy = endY - startY;
    float length = sqrt(dx * dx + dy * dy);
    
    if (length > 0.001f) {
        dx /= length;
        dy /= length;
    } else {
        dx = 1.0f;
        dy = 0.0f;
    }
    
    // Calculate perpendicular direction
    float px = -dy;
    float py = dx;
    
    // Arrow dimensions
    float arrowLength = 0.1f * scale;
    float arrowWidth = 0.03f * scale;
    float headSize = 0.05f * scale;
    
    // Arrow body vertices
    float tailX = x - dx * arrowLength;
    float tailY = y - dy * arrowLength;
    
    // Arrow body
    Vertex v1 = {{tailX - px * arrowWidth, tailY - py * arrowWidth}, {ARROW_COLOR[0], ARROW_COLOR[1], ARROW_COLOR[2]}};
    Vertex v2 = {{tailX + px * arrowWidth, tailY + py * arrowWidth}, {ARROW_COLOR[0], ARROW_COLOR[1], ARROW_COLOR[2]}};
    Vertex v3 = {{x - px * arrowWidth, y - py * arrowWidth}, {ARROW_COLOR[0], ARROW_COLOR[1], ARROW_COLOR[2]}};
    Vertex v4 = {{x + px * arrowWidth, y + py * arrowWidth}, {ARROW_COLOR[0], ARROW_COLOR[1], ARROW_COLOR[2]}};
    
    // Arrow head
    Vertex v5 = {{x, y}, {ARROW_COLOR[0], ARROW_COLOR[1], ARROW_COLOR[2]}};
    Vertex v6 = {{x + dx * headSize, y + dy * headSize}, {ARROW_COLOR[0], ARROW_COLOR[1], ARROW_COLOR[2]}};
    Vertex v7 = {{x - px * headSize, y - py * headSize}, {ARROW_COLOR[0], ARROW_COLOR[1], ARROW_COLOR[2]}};
    Vertex v8 = {{x + px * headSize, y + py * headSize}, {ARROW_COLOR[0], ARROW_COLOR[1], ARROW_COLOR[2]}};
    
    // Body triangles
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    
    vertices.push_back(v2);
    vertices.push_back(v4);
    vertices.push_back(v3);
    
    // Head triangles
    vertices.push_back(v5);
    vertices.push_back(v7);
    vertices.push_back(v6);
    
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v8);
    
    return vertices;
}

std::vector<Vertex> VisualEffect::generateFireballVertices() const {
    std::vector<Vertex> vertices;
    
    if (lifetime <= 0.0f) return vertices;
    
    // Calculate position (interpolate from start to end)
    float progress = 1.0f - (lifetime / maxLifetime);
    float x = startX + (endX - startX) * progress;
    float y = startY + (endY - startY) * progress;
    
    // Calculate alpha based on lifetime
    float alpha = lifetime / maxLifetime;
    
    // Create a fireball (circle with inner and outer parts)
    float outerRadius = 0.12f * scale;
    float innerRadius = 0.08f * scale;
    int segments = 12;
    
    // Outer circle (orange-red)
    for (int i = 0; i < segments; i++) {
        float angle1 = 2.0f * 3.14159f * i / segments;
        float angle2 = 2.0f * 3.14159f * (i + 1) / segments;
        
        float x1 = x + cos(angle1) * outerRadius;
        float y1 = y + sin(angle1) * outerRadius;
        float x2 = x + cos(angle2) * outerRadius;
        float y2 = y + sin(angle2) * outerRadius;
        
        Vertex v1 = {{x, y}, {FIREBALL_COLOR[0], FIREBALL_COLOR[1], FIREBALL_COLOR[2]}};
        Vertex v2 = {{x1, y1}, {FIREBALL_COLOR[0], FIREBALL_COLOR[1], FIREBALL_COLOR[2]}};
        Vertex v3 = {{x2, y2}, {FIREBALL_COLOR[0], FIREBALL_COLOR[1], FIREBALL_COLOR[2]}};
        
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
    }
    
    // Inner circle (brighter)
    for (int i = 0; i < segments; i++) {
        float angle1 = 2.0f * 3.14159f * i / segments;
        float angle2 = 2.0f * 3.14159f * (i + 1) / segments;
        
        float x1 = x + cos(angle1) * innerRadius;
        float y1 = y + sin(angle1) * innerRadius;
        float x2 = x + cos(angle2) * innerRadius;
        float y2 = y + sin(angle2) * innerRadius;
        
        Vertex v1 = {{x, y}, {1.0f, 0.9f, 0.3f}}; // Bright yellow core
        Vertex v2 = {{x1, y1}, {1.0f, 0.7f, 0.2f}};
        Vertex v3 = {{x2, y2}, {1.0f, 0.7f, 0.2f}};
        
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
    }
    
    return vertices;
}

std::shared_ptr<VisualEffect> VisualEffect::createEffect(Character::VisualEffectType type, 
                                                        float startX, float startY, 
                                                        float endX, float endY) {
    return std::make_shared<VisualEffect>(type, startX, startY, endX, endY);
}

// VisualEffectManager implementation
VisualEffectManager::VisualEffectManager() {
}

VisualEffectManager::~VisualEffectManager() {
    clear();
}

void VisualEffectManager::addEffect(Character::VisualEffectType type, float startX, float startY, float endX, float endY) {
    auto effect = VisualEffect::createEffect(type, startX, startY, endX, endY);
    activeEffects.push_back(effect);
}

void VisualEffectManager::update(float deltaTime) {
    // Update all effects
    for (auto& effect : activeEffects) {
        effect->update(deltaTime);
    }
    
    // Remove finished effects
    activeEffects.erase(
        std::remove_if(activeEffects.begin(), activeEffects.end(),
                      [](const std::shared_ptr<VisualEffect>& effect) {
                          return effect->isFinished();
                      }),
        activeEffects.end()
    );
}

std::vector<Vertex> VisualEffectManager::generateAllEffectVertices() const {
    std::vector<Vertex> allVertices;
    
    for (const auto& effect : activeEffects) {
        auto effectVertices = effect->generateVertices();
        allVertices.insert(allVertices.end(), effectVertices.begin(), effectVertices.end());
    }
    
    return allVertices;
}

void VisualEffectManager::clear() {
    activeEffects.clear();
}
