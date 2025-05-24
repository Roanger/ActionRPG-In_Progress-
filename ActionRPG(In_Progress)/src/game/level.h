#pragma once

#include <vector>
#include <memory>
#include <string>
#include "character.h"
#include "enemy.h"
#include "item.h"
#include "item_drop.h"

enum class TileType {
    FLOOR,
    WALL,
    DOOR,
    WATER,
    LAVA
};

struct Tile {
    TileType type;
    bool explored;
    bool visible;
};

class Level {
public:
    Level(int width, int height);
    ~Level();
    
    // Level generation
    void generateDungeon();
    void generateForest();
    void generateCave();
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    Tile getTile(int x, int y) const;
    const std::vector<std::shared_ptr<Enemy>>& getEnemies() const { return enemies; }
    const std::vector<std::shared_ptr<Item>>& getItems() const { return items; }
    ItemDropManager& getItemDropManager() { return itemDropManager; }
    
    // Level manipulation
    void setTile(int x, int y, TileType type);
    void addEnemy(std::shared_ptr<Enemy> enemy);
    void removeEnemy(std::shared_ptr<Enemy> enemy);
    void addItem(std::shared_ptr<Item> item, float x, float y);
    int getPickupItemIndex(float playerX, float playerY) const;
    std::shared_ptr<Item> pickupItem(int index);
    
    // Game logic
    void update(float deltaTime, Character* player);
    bool isWalkable(int x, int y) const;
    
private:
    int width;
    int height;
    std::vector<Tile> tiles;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::vector<std::shared_ptr<Item>> items; // Added this line
    ItemDropManager itemDropManager;
    
    // Helper methods for level generation
    void createRoom(int x1, int y1, int x2, int y2);
    void createCorridor(int x1, int y1, int x2, int y2);
    
    // Enemy drop system
    void dropLoot(float x, float y, int enemyLevel);
}; 