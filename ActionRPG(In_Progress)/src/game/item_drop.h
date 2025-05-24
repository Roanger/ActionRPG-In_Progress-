#pragma once

#include <memory>
#include <vector>
#include "item.h"
#include "../engine/vertex.h"

// Represents an item that exists in the game world and can be picked up
class ItemDrop {
public:
    ItemDrop(std::shared_ptr<Item> item, float x, float y);
    ~ItemDrop();
    
    // Getters
    std::shared_ptr<Item> getItem() const { return item; }
    float getX() const { return x; }
    float getY() const { return y; }
    
    // Check if player is close enough to pick up the item
    bool canPickup(float playerX, float playerY) const;
    
    // Visual effects
    void update(float deltaTime);
    std::vector<Vertex> generateVertices() const;
    
private:
    std::shared_ptr<Item> item;
    float x, y;
    
    // Visual properties
    float hoverOffset;
    float hoverDirection;
    float rotationAngle;
    
    // Constants
    static constexpr float PICKUP_RADIUS = 1.0f;
    static constexpr float HOVER_SPEED = 0.5f;
    static constexpr float HOVER_AMPLITUDE = 0.05f;
    static constexpr float ROTATION_SPEED = 1.0f;
};

// Class to manage all item drops in the game
class ItemDropManager {
public:
    ItemDropManager();
    ~ItemDropManager();
    
    // Add a new item drop to the world
    void addItemDrop(std::shared_ptr<Item> item, float x, float y);
    
    // Remove an item drop (when picked up)
    void removeItemDrop(size_t index);
    
    // Update all item drops
    void update(float deltaTime);
    
    // Generate vertices for rendering all item drops
    std::vector<Vertex> generateAllItemVertices() const;
    
    // Check if player can pick up any items and return the index if possible
    int getPickupItemIndex(float playerX, float playerY) const;
    
    // Get all item drops
    const std::vector<std::shared_ptr<ItemDrop>>& getItemDrops() const { return itemDrops; }
    
private:
    std::vector<std::shared_ptr<ItemDrop>> itemDrops;
};
