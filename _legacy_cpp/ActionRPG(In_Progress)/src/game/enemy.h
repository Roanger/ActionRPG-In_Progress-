#pragma once

#include "character.h"
#include <string>
#include <random>

enum class EnemyType {
    GOBLIN,
    SKELETON,
    ORC,
    TROLL,
    DRAGON
};

class Level; // Forward declaration

class Enemy : public Character {
public:
    Enemy(const std::string& name, EnemyType type, int level);
    ~Enemy();
    
    EnemyType getEnemyType() const { return enemyType; }
    int getExperienceReward() const { return experienceReward; }
    int getGoldReward() const { return goldReward; }
    
    // Set the level reference for collision detection
    void setLevel(Level* level) { currentLevel = level; }
    
    // AI behavior
    void update(float deltaTime, Character* player);
    
private:
    EnemyType enemyType;
    int experienceReward;
    int goldReward;
    float attackCooldown;
    float currentCooldown;
    Level* currentLevel = nullptr; // Reference to the level for collision detection
    float movementCooldown = 0.8f; // Time between enemy movements (Increased to slow them down)
    float movementTimer = 0.0f;    // Current movement timer
    float debugTimer = 0.0f;       // Timer for debug output
    
    void initializeByType();
}; 