#include "level.h"
#include <algorithm>
#include <random>
#include <ctime>
#include <iostream>

Level::Level(int width, int height) : width(width), height(height) {
    // Initialize all tiles as walls
    tiles.resize(width * height);
    for (auto& tile : tiles) {
        tile.type = TileType::WALL;
        tile.explored = false;
        tile.visible = false;
    }
}

Level::~Level() {
}

Tile Level::getTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        // Return wall for out of bounds
        Tile wall;
        wall.type = TileType::WALL;
        wall.explored = false;
        wall.visible = false;
        return wall;
    }
    
    return tiles[y * width + x];
}

void Level::setTile(int x, int y, TileType type) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        tiles[y * width + x].type = type;
    }
}

bool Level::isWalkable(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    
    TileType type = tiles[y * width + x].type;
    return type == TileType::FLOOR || type == TileType::DOOR;
}

void Level::generateDungeon() {
    // Simple dungeon generation algorithm
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    
    // Create a few random rooms
    int numRooms = 5 + rng() % 10;
    
    for (int i = 0; i < numRooms; i++) {
        int roomWidth = 3 + rng() % 8;
        int roomHeight = 3 + rng() % 6;
        int x = rng() % (width - roomWidth - 2) + 1;
        int y = rng() % (height - roomHeight - 2) + 1;
        
        createRoom(x, y, x + roomWidth, y + roomHeight);
        
        // Connect to previous room
        if (i > 0) {
            // Get center of current room
            int newX = x + roomWidth / 2;
            int newY = y + roomHeight / 2;
            
            // Get center of previous room
            int prevX = rng() % (width - 4) + 2;
            int prevY = rng() % (height - 4) + 2;
            
            // Create corridor between rooms
            if (rng() % 2 == 0) {
                createCorridor(prevX, prevY, newX, prevY);
                createCorridor(newX, prevY, newX, newY);
            } else {
                createCorridor(prevX, prevY, prevX, newY);
                createCorridor(prevX, newY, newX, newY);
            }
        }
        
        // Add enemies to the room
        int numEnemies = rng() % 3;
        for (int j = 0; j < numEnemies; j++) {
            int enemyX = x + 1 + rng() % (roomWidth - 2);
            int enemyY = y + 1 + rng() % (roomHeight - 2);
            
            // Create random enemy type based on level depth
            EnemyType type;
            int roll = rng() % 100;
            if (roll < 50) {
                type = EnemyType::GOBLIN;
            } else if (roll < 80) {
                type = EnemyType::SKELETON;
            } else if (roll < 95) {
                type = EnemyType::ORC;
            } else {
                type = EnemyType::TROLL;
            }
            
            // Create enemy with random level (1-3)
            int enemyLevel = 1 + rng() % 3;
            std::string enemyName;
            
            switch (type) {
                case EnemyType::GOBLIN:
                    enemyName = "Goblin";
                    break;
                case EnemyType::SKELETON:
                    enemyName = "Skeleton";
                    break;
                case EnemyType::ORC:
                    enemyName = "Orc";
                    break;
                case EnemyType::TROLL:
                    enemyName = "Troll";
                    break;
                case EnemyType::DRAGON:
                    enemyName = "Dragon";
                    break;
            }
            
            auto enemy = std::make_shared<Enemy>(enemyName, type, enemyLevel);
            enemy->move(static_cast<float>(enemyX), static_cast<float>(enemyY));
            addEnemy(enemy);
        }
        
        // Add items to the room
        if (rng() % 3 == 0) {
            int itemX = x + 1 + rng() % (roomWidth - 2);
            int itemY = y + 1 + rng() % (roomHeight - 2);
            
            // Create random item
            int itemRoll = rng() % 100;
            std::shared_ptr<Item> item;
            
            if (itemRoll < 40) {
                // Create potion
                item = std::make_shared<Potion>("Health Potion", ItemRarity::COMMON, 20);
            } else if (itemRoll < 70) {
                // Create weapon
                item = std::make_shared<Weapon>("Iron Sword", ItemRarity::UNCOMMON, 10);
            } else {
                // Create armor
                item = std::make_shared<Armor>("Leather Armor", ItemRarity::UNCOMMON, 5);
            }
            
            addItem(item, static_cast<float>(itemX), static_cast<float>(itemY));
        }
    }
}

void Level::createRoom(int x1, int y1, int x2, int y2) {
    // Ensure coordinates are within bounds
    x1 = std::max(1, std::min(width - 2, x1));
    y1 = std::max(1, std::min(height - 2, y1));
    x2 = std::max(1, std::min(width - 2, x2));
    y2 = std::max(1, std::min(height - 2, y2));
    
    // Create floor tiles for the room
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            setTile(x, y, TileType::FLOOR);
            
            // Mark the tile as explored and visible
            if (x >= 0 && x < width && y >= 0 && y < height) {
                tiles[y * width + x].explored = true;
                tiles[y * width + x].visible = true;
            }
        }
    }
}

void Level::createCorridor(int x1, int y1, int x2, int y2) {
    // Create a corridor from (x1,y1) to (x2,y2)
    for (int x = std::min(x1, x2); x <= std::max(x1, x2); x++) {
        setTile(x, y1, TileType::FLOOR);
        
        // Mark the tile as explored and visible
        if (x >= 0 && x < width && y1 >= 0 && y1 < height) {
            tiles[y1 * width + x].explored = true;
            tiles[y1 * width + x].visible = true;
        }
    }
    
    for (int y = std::min(y1, y2); y <= std::max(y1, y2); y++) {
        setTile(x2, y, TileType::FLOOR);
        
        // Mark the tile as explored and visible
        if (x2 >= 0 && x2 < width && y >= 0 && y < height) {
            tiles[y * width + x2].explored = true;
            tiles[y * width + x2].visible = true;
        }
    }
}

void Level::addEnemy(std::shared_ptr<Enemy> enemy) {
    // Set the level reference for the enemy
    enemy->setLevel(this);
    enemies.push_back(enemy);
}

void Level::removeEnemy(std::shared_ptr<Enemy> enemy) {
    auto it = std::find(enemies.begin(), enemies.end(), enemy);
    if (it != enemies.end()) {
        enemies.erase(it);
    }
}

void Level::addItem(std::shared_ptr<Item> item, float x, float y) {
    itemDropManager.addItemDrop(item, x, y);
}

int Level::getPickupItemIndex(float playerX, float playerY) const {
    return itemDropManager.getPickupItemIndex(playerX, playerY);
}

std::shared_ptr<Item> Level::pickupItem(int index) {
    if (index < 0) {
        return nullptr;
    }
    
    // Get the item before removing it from the manager
    auto itemDrops = itemDropManager.getItemDrops();
    if (index >= static_cast<int>(itemDrops.size())) {
        return nullptr;
    }
    
    auto item = itemDrops[index]->getItem();
    itemDropManager.removeItemDrop(index);
    return item;
}

void Level::update(float deltaTime, Character* player) {
    // Update all enemies
    for (auto it = enemies.begin(); it != enemies.end();) {
        auto& enemy = *it;
        
        // Update enemy
        enemy->update(deltaTime, player);
        
        // Check if enemy is dead
        if (enemy->isDead()) {
            // Drop loot at enemy position
            dropLoot(enemy->getX(), enemy->getY(), enemy->getLevel());
            
            // Remove dead enemy
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
    
    // Update item drops (animations, etc.)
    itemDropManager.update(deltaTime);
}

void Level::dropLoot(float x, float y, int enemyLevel) {
    // Random number generator
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_real_distribution<float> dropChance(0.0f, 1.0f);
    std::uniform_int_distribution<int> itemTypeRoll(0, 2); // 0=Weapon, 1=Armor, 2=Potion
    std::uniform_int_distribution<int> rarityRoll(0, 100);
    
    // 50% chance to drop an item
    if (dropChance(rng) < 0.5f) {
        // Determine item type
        int itemType = itemTypeRoll(rng);
        
        // Determine rarity based on enemy level
        ItemRarity rarity;
        int rarityValue = rarityRoll(rng) + enemyLevel * 5;
        
        if (rarityValue > 95) {
            rarity = ItemRarity::LEGENDARY;
        } else if (rarityValue > 80) {
            rarity = ItemRarity::EPIC;
        } else if (rarityValue > 60) {
            rarity = ItemRarity::RARE;
        } else if (rarityValue > 30) {
            rarity = ItemRarity::UNCOMMON;
        } else {
            rarity = ItemRarity::COMMON;
        }
        
        // Create the item based on type
        std::shared_ptr<Item> item;
        
        switch (itemType) {
            case 0: { // Weapon
                int damage = 5 + enemyLevel * 2;
                if (rarity == ItemRarity::UNCOMMON) damage += 3;
                if (rarity == ItemRarity::RARE) damage += 7;
                if (rarity == ItemRarity::EPIC) damage += 12;
                if (rarity == ItemRarity::LEGENDARY) damage += 20;
                
                item = std::make_shared<Weapon>("Sword", rarity, damage);
                break;
            }
            case 1: { // Armor
                int defense = 3 + enemyLevel;
                if (rarity == ItemRarity::UNCOMMON) defense += 2;
                if (rarity == ItemRarity::RARE) defense += 4;
                if (rarity == ItemRarity::EPIC) defense += 7;
                if (rarity == ItemRarity::LEGENDARY) defense += 12;
                
                item = std::make_shared<Armor>("Armor", rarity, defense);
                break;
            }
            case 2: { // Potion
                int healAmount = 10 + enemyLevel * 3;
                if (rarity == ItemRarity::UNCOMMON) healAmount += 5;
                if (rarity == ItemRarity::RARE) healAmount += 15;
                if (rarity == ItemRarity::EPIC) healAmount += 30;
                if (rarity == ItemRarity::LEGENDARY) healAmount += 50;
                
                item = std::make_shared<Potion>("Potion", rarity, healAmount);
                break;
            }
        }
        
        // Add the item to the world
        if (item) {
            addItem(item, x, y);
        }
    }
}