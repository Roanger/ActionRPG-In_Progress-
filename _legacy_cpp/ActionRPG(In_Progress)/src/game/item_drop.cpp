#include "item_drop.h"
#include <cmath>
#include <iostream>

// ItemDrop implementation
ItemDrop::ItemDrop(std::shared_ptr<Item> item, float x, float y)
    : item(item), x(x), y(y), hoverOffset(0.0f), hoverDirection(1.0f), rotationAngle(0.0f) {
}

ItemDrop::~ItemDrop() {
}

bool ItemDrop::canPickup(float playerX, float playerY) const {
    float dx = playerX - x;
    float dy = playerY - y;
    float distanceSquared = dx * dx + dy * dy;
    
    return distanceSquared <= PICKUP_RADIUS * PICKUP_RADIUS;
}

void ItemDrop::update(float deltaTime) {
    // Update hover animation
    hoverOffset += hoverDirection * HOVER_SPEED * deltaTime;
    
    // Reverse direction if limits reached
    if (hoverOffset > HOVER_AMPLITUDE || hoverOffset < -HOVER_AMPLITUDE) {
        hoverDirection *= -1.0f;
    }
    
    // Update rotation
    rotationAngle += ROTATION_SPEED * deltaTime;
    if (rotationAngle > 6.28318f) { // 2*PI
        rotationAngle -= 6.28318f;
    }
}

std::vector<Vertex> ItemDrop::generateVertices() const {
    std::vector<Vertex> vertices;
    
    // Get item color based on rarity
    float r = 1.0f, g = 1.0f, b = 1.0f; // Default white
    
    switch (item->getRarity()) {
        case ItemRarity::COMMON:
            r = 0.8f; g = 0.8f; b = 0.8f; // Gray
            break;
        case ItemRarity::UNCOMMON:
            r = 0.0f; g = 0.8f; b = 0.0f; // Green
            break;
        case ItemRarity::RARE:
            r = 0.0f; g = 0.0f; b = 1.0f; // Blue
            break;
        case ItemRarity::EPIC:
            r = 0.8f; g = 0.0f; b = 0.8f; // Purple
            break;
        case ItemRarity::LEGENDARY:
            r = 1.0f; g = 0.6f; b = 0.0f; // Orange
            break;
    }
    
    // Apply hover effect to y position
    float itemY = y + hoverOffset;
    
    // Item shape depends on type
    float size = 0.15f;
    
    // Apply rotation for visual interest
    float cosR = cos(rotationAngle);
    float sinR = sin(rotationAngle);
    
    switch (item->getType()) {
        case ItemType::WEAPON: {
            // Sword shape (elongated rectangle)
            float width = size * 0.3f;
            float height = size * 1.0f;
            
            // Define corners with rotation
            float x1 = -width, y1 = -height;
            float x2 = width, y2 = -height;
            float x3 = -width, y3 = height;
            float x4 = width, y4 = height;
            
            // Rotate points
            float rx1 = x1 * cosR - y1 * sinR;
            float ry1 = x1 * sinR + y1 * cosR;
            float rx2 = x2 * cosR - y2 * sinR;
            float ry2 = x2 * sinR + y2 * cosR;
            float rx3 = x3 * cosR - y3 * sinR;
            float ry3 = x3 * sinR + y3 * cosR;
            float rx4 = x4 * cosR - y4 * sinR;
            float ry4 = x4 * sinR + y4 * cosR;
            
            // Create vertices
            Vertex v1 = {{x + rx1, itemY + ry1}, {r, g, b}};
            Vertex v2 = {{x + rx2, itemY + ry2}, {r, g, b}};
            Vertex v3 = {{x + rx3, itemY + ry3}, {r, g, b}};
            Vertex v4 = {{x + rx4, itemY + ry4}, {r, g, b}};
            
            // First triangle
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
            // Second triangle
            vertices.push_back(v2);
            vertices.push_back(v4);
            vertices.push_back(v3);
            break;
        }
        
        case ItemType::ARMOR: {
            // Shield shape (shield-like quadrilateral)
            float width = size * 0.8f;
            float height = size * 1.0f;
            
            // Define corners with rotation
            float x1 = -width, y1 = -height * 0.5f;
            float x2 = width, y2 = -height * 0.5f;
            float x3 = -width * 0.5f, y3 = height * 0.5f;
            float x4 = width * 0.5f, y4 = height * 0.5f;
            
            // Rotate points
            float rx1 = x1 * cosR - y1 * sinR;
            float ry1 = x1 * sinR + y1 * cosR;
            float rx2 = x2 * cosR - y2 * sinR;
            float ry2 = x2 * sinR + y2 * cosR;
            float rx3 = x3 * cosR - y3 * sinR;
            float ry3 = x3 * sinR + y3 * cosR;
            float rx4 = x4 * cosR - y4 * sinR;
            float ry4 = x4 * sinR + y4 * cosR;
            
            // Create vertices
            Vertex v1 = {{x + rx1, itemY + ry1}, {r, g, b}};
            Vertex v2 = {{x + rx2, itemY + ry2}, {r, g, b}};
            Vertex v3 = {{x + rx3, itemY + ry3}, {r, g, b}};
            Vertex v4 = {{x + rx4, itemY + ry4}, {r, g, b}};
            
            // First triangle
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
            // Second triangle
            vertices.push_back(v2);
            vertices.push_back(v4);
            vertices.push_back(v3);
            break;
        }
        
        case ItemType::POTION: {
            // Potion shape (circular)
            int segments = 8;
            float radius = size * 0.6f;
            
            for (int i = 0; i < segments; i++) {
                float angle1 = 2.0f * 3.14159f * i / segments;
                float angle2 = 2.0f * 3.14159f * (i + 1) / segments;
                
                float x1 = x;
                float y1 = itemY;
                float x2 = x + radius * cos(angle1);
                float y2 = itemY + radius * sin(angle1);
                float x3 = x + radius * cos(angle2);
                float y3 = itemY + radius * sin(angle2);
                
                Vertex v1 = {{x1, y1}, {r, g, b}};
                Vertex v2 = {{x2, y2}, {r, g, b}};
                Vertex v3 = {{x3, y3}, {r, g, b}};
                
                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v3);
            }
            break;
        }
        
        default: {
            // Default square shape for other items
            float halfSize = size * 0.5f;
            
            // Define corners with rotation
            float x1 = -halfSize, y1 = -halfSize;
            float x2 = halfSize, y2 = -halfSize;
            float x3 = -halfSize, y3 = halfSize;
            float x4 = halfSize, y4 = halfSize;
            
            // Rotate points
            float rx1 = x1 * cosR - y1 * sinR;
            float ry1 = x1 * sinR + y1 * cosR;
            float rx2 = x2 * cosR - y2 * sinR;
            float ry2 = x2 * sinR + y2 * cosR;
            float rx3 = x3 * cosR - y3 * sinR;
            float ry3 = x3 * sinR + y3 * cosR;
            float rx4 = x4 * cosR - y4 * sinR;
            float ry4 = x4 * sinR + y4 * cosR;
            
            // Create vertices
            Vertex v1 = {{x + rx1, itemY + ry1}, {r, g, b}};
            Vertex v2 = {{x + rx2, itemY + ry2}, {r, g, b}};
            Vertex v3 = {{x + rx3, itemY + ry3}, {r, g, b}};
            Vertex v4 = {{x + rx4, itemY + ry4}, {r, g, b}};
            
            // First triangle
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
            // Second triangle
            vertices.push_back(v2);
            vertices.push_back(v4);
            vertices.push_back(v3);
            break;
        }
    }
    
    return vertices;
}

// ItemDropManager implementation
ItemDropManager::ItemDropManager() {
}

ItemDropManager::~ItemDropManager() {
}

void ItemDropManager::addItemDrop(std::shared_ptr<Item> item, float x, float y) {
    auto itemDrop = std::make_shared<ItemDrop>(item, x, y);
    itemDrops.push_back(itemDrop);
    std::cout << "Item dropped: " << item->getName() << " at position (" << x << ", " << y << ")" << std::endl;
}

void ItemDropManager::removeItemDrop(size_t index) {
    if (index < itemDrops.size()) {
        auto item = itemDrops[index]->getItem();
        std::cout << "Item picked up: " << item->getName() << std::endl;
        
        itemDrops.erase(itemDrops.begin() + index);
    }
}

void ItemDropManager::update(float deltaTime) {
    for (auto& itemDrop : itemDrops) {
        itemDrop->update(deltaTime);
    }
}

std::vector<Vertex> ItemDropManager::generateAllItemVertices() const {
    std::vector<Vertex> allVertices;
    
    for (const auto& itemDrop : itemDrops) {
        auto vertices = itemDrop->generateVertices();
        allVertices.insert(allVertices.end(), vertices.begin(), vertices.end());
    }
    
    return allVertices;
}

int ItemDropManager::getPickupItemIndex(float playerX, float playerY) const {
    for (size_t i = 0; i < itemDrops.size(); i++) {
        if (itemDrops[i]->canPickup(playerX, playerY)) {
            return static_cast<int>(i);
        }
    }
    
    return -1; // No item in range
}
