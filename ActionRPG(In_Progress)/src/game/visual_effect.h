#pragma once

#include <vector>
#include <memory>
#include "character.h"
#include "../engine/vertex.h"

class VisualEffect {
public:
    VisualEffect(Character::VisualEffectType type, float startX, float startY, float endX, float endY);
    ~VisualEffect();
    
    void update(float deltaTime);
    std::vector<Vertex> generateVertices() const;
    bool isFinished() const { return lifetime <= 0.0f; }
    
    // Static factory method to create effects
    static std::shared_ptr<VisualEffect> createEffect(Character::VisualEffectType type, 
                                                     float startX, float startY, 
                                                     float endX, float endY);
    
private:
    Character::VisualEffectType type;
    float startX, startY;
    float endX, endY;
    float lifetime;
    float maxLifetime;
    float scale;
    float rotation;
    
    // Colors for different effect types
    static const float SLASH_COLOR[3];
    static const float ARROW_COLOR[3];
    static const float FIREBALL_COLOR[3];
    
    // Generate vertices for different effect types
    std::vector<Vertex> generateSlashVertices() const;
    std::vector<Vertex> generateArrowVertices() const;
    std::vector<Vertex> generateFireballVertices() const;
};

// Class to manage all active visual effects
class VisualEffectManager {
public:
    VisualEffectManager();
    ~VisualEffectManager();
    
    void addEffect(Character::VisualEffectType type, float startX, float startY, float endX, float endY);
    void update(float deltaTime);
    std::vector<Vertex> generateAllEffectVertices() const;
    const std::vector<std::shared_ptr<VisualEffect>>& getEffects() const { return activeEffects; }
    void clear();

private:
    std::vector<std::shared_ptr<VisualEffect>> activeEffects;
};
