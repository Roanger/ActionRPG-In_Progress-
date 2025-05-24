#include "enemy.h"
#include "level.h"
#include <iostream>
#include <cmath>

Enemy::Enemy(const std::string& name, EnemyType type, int level)
    : Character(name, CharacterClass::WARRIOR), // Default to warrior stats
      enemyType(type),
      attackCooldown(2.0f),
      currentCooldown(0.0f) {
    
    // Randomize initial movement timer to prevent enemies from moving in sync
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 0.5f);
    movementTimer = dist(gen);
    
    // Set level
    for (int i = 1; i < level; i++) {
        levelUp();
    }
    
    // Initialize based on enemy type
    initializeByType();
    
    // Calculate rewards based on level
    experienceReward = level * 25;
    goldReward = level * 10;
}

Enemy::~Enemy() {
}

void Enemy::initializeByType() {
    // Adjust stats based on enemy type
    switch (enemyType) {
        case EnemyType::GOBLIN:
            // Fast but weak
            strength -= 2;
            dexterity += 4;
            maxHealth = static_cast<int>(maxHealth * 0.8);
            health = maxHealth;
            attackCooldown = 1.5f;
            break;
            
        case EnemyType::SKELETON:
            // Balanced
            dexterity += 2;
            maxHealth = static_cast<int>(maxHealth * 0.9);
            health = maxHealth;
            break;
            
        case EnemyType::ORC:
            // Strong but slow
            strength += 4;
            dexterity -= 2;
            maxHealth = static_cast<int>(maxHealth * 1.2);
            health = maxHealth;
            attackCooldown = 2.5f;
            break;
            
        case EnemyType::TROLL:
            // Very strong, very slow, high health
            strength += 8;
            dexterity -= 4;
            maxHealth = static_cast<int>(maxHealth * 2.0);
            health = maxHealth;
            attackCooldown = 3.0f;
            break;
            
        case EnemyType::DRAGON:
            // Boss enemy
            strength += 15;
            dexterity += 5;
            intelligence += 10;
            maxHealth = static_cast<int>(maxHealth * 5.0);
            health = maxHealth;
            attackCooldown = 4.0f;
            
            // Increase rewards for dragon
            experienceReward *= 5;
            goldReward *= 10;
            break;
    }
}

void Enemy::update(float deltaTime, Character* player) {
    // Skip update if player is null
    if (!player) {
        return;
    }
    
    // Always ensure we have a level reference
    if (!currentLevel) {
        std::cerr << "Enemy " << getName() << " has no level reference!" << std::endl;
        return;
    }
    
    // Update cooldowns
    if (currentCooldown > 0) {
        currentCooldown -= deltaTime;
    }
    
    // Always update movement timer
    movementTimer -= deltaTime;
    
    // Calculate distance to player
    float dx = player->getX() - getX();
    float dy = player->getY() - getY();
    float distance = std::sqrt(dx * dx + dy * dy);
    
    // Debug output occasionally
    debugTimer += deltaTime;
    if (debugTimer > 5.0f) {
        debugTimer = 0.0f;
        std::cout << "Enemy " << getName() << " at (" << getX() << ", " << getY() 
                  << "), distance to player: " << distance 
                  << ", movement timer: " << movementTimer << std::endl;
    }
    
    // Attack if in range
    if (distance < 1.5f) {
        if (currentCooldown <= 0) {
            // Attack player
            attack(player);
            currentCooldown = attackCooldown;
            
            // Debug attack
            std::cout << "Enemy " << getName() << " attacking player!" << std::endl;
        }
    } 
    // Always try to move if not attacking and within chase radius
    else if (distance <= 10.0f) { // Increased to 10-block radius as requested
        // Only wait for movement timer if we're very close to the player
        if (movementTimer <= 0.0f || distance > 3.0f) {
            // Debug output
            std::cout << "Enemy " << getName() << " chasing player, distance: " << distance << std::endl;
            
            // Always move towards player within chase radius
            float moveX = 0.0f;
            float moveY = 0.0f;
            
            // Normalize direction vector
            if (distance > 0) {
                moveX = dx / distance;
                moveY = dy / distance;
            }
            
            // Round to get grid-based movement (0, 1, or -1)
            moveX = moveX > 0.3f ? 1.0f : (moveX < -0.3f ? -1.0f : 0.0f);
            moveY = moveY > 0.3f ? 1.0f : (moveY < -0.3f ? -1.0f : 0.0f);
            
            // Make sure we have a valid movement direction
            if (moveX == 0.0f && moveY == 0.0f) {
                // Force a movement direction if both are zero
                moveX = (dx >= 0) ? 1.0f : -1.0f;
            }
            
            // Calculate new position
            float newX = getX() + moveX;
            float newY = getY() + moveY;
            
            // Check if new position is walkable
            int tileX = static_cast<int>(std::round(newX));
            int tileY = static_cast<int>(std::round(newY));
            
            bool moved = false;
            
            if (currentLevel->isWalkable(tileX, tileY)) {
                // Move enemy to the new position
                move(newX, newY);
                moved = true;
            } else {
                // If we can't move in the desired direction, try alternative directions
                // Try horizontal movement first if vertical is blocked
                if (moveY != 0.0f) {
                    newY = getY(); // Keep Y the same
                    tileY = static_cast<int>(std::round(newY));
                    
                    if (currentLevel->isWalkable(tileX, tileY)) {
                        move(newX, newY);
                        moved = true;
                    }
                }
                
                // Try vertical movement if horizontal is blocked or was also blocked
                if (!moved && moveX != 0.0f) {
                    newX = getX(); // Keep X the same
                    newY = getY() + moveY; // Use original moveY
                    tileX = static_cast<int>(std::round(newX));
                    tileY = static_cast<int>(std::round(newY));
                    
                    if (currentLevel->isWalkable(tileX, tileY)) {
                        move(newX, newY);
                        moved = true;
                    }
                }
                
                // If still blocked, try random directions
                if (!moved) {
                    // Try up to 8 directions (all surrounding tiles)
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            if (dx == 0 && dy == 0) continue; // Skip center
                            
                            newX = getX() + dx;
                            newY = getY() + dy;
                            tileX = static_cast<int>(std::round(newX));
                            tileY = static_cast<int>(std::round(newY));
                            
                            if (currentLevel->isWalkable(tileX, tileY)) {
                                move(newX, newY);
                                moved = true;
                                break;
                            }
                        }
                        if (moved) break;
                    }
                }
            }
            
            // Set a shorter cooldown if we're far from the player to move faster
            if (distance > 5.0f) {
                movementTimer = movementCooldown * 0.5f; // Move twice as fast when far away
            } else {
                movementTimer = movementCooldown;
            }
        }
    } else {
        // Random movement when outside chase radius (> 10 blocks)
        if (movementTimer <= 0.0f) {
            // Use non-static RNG for random movement
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dir(-1, 1);
            
            float moveX = static_cast<float>(dir(gen));
            float moveY = static_cast<float>(dir(gen));
            
            // Ensure we have movement
            if (moveX == 0.0f && moveY == 0.0f) {
                moveX = static_cast<float>(dir(gen) ? 1 : -1);
            }
            
            // Calculate new position
            float newX = getX() + moveX;
            float newY = getY() + moveY;
            int tileX = static_cast<int>(std::round(newX));
            int tileY = static_cast<int>(std::round(newY));
            
            if (currentLevel->isWalkable(tileX, tileY)) {
                move(newX, newY);
            }
            
            // Longer cooldown for random movement
            movementTimer = movementCooldown * 1.5f;
        }
    }
} 