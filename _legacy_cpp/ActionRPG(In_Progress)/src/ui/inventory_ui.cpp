#include "inventory_ui.h"
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>

InventoryUI::InventoryUI() 
    : isVisible(false), selectedItemIndex(0), animationTimer(0.0f) {
}

InventoryUI::~InventoryUI() {
}

void InventoryUI::update(float deltaTime) {
    // Update animation timer for visual effects
    animationTimer += deltaTime;
    if (animationTimer > 6.28318f) { // 2*PI
        animationTimer -= 6.28318f;
    }
}

std::vector<Vertex> InventoryUI::generateInventoryVertices(const std::shared_ptr<Character>& player) const {
    std::vector<Vertex> vertices;
    
    // Only generate vertices if the inventory is visible
    if (!isVisible) {
        return vertices;
    }
    
    // Get player's inventory
    const auto& inventory = player->getInventory();
    
    // Combine all UI elements
    auto background = createInventoryBackground();
    auto slots = createItemSlots();
    auto icons = createItemIcons(inventory);
    auto highlight = createSelectionHighlight();
    
    // If an item is selected, show its details
    std::vector<Vertex> details;
    if (!inventory.empty() && selectedItemIndex >= 0 && selectedItemIndex < static_cast<int>(inventory.size())) {
        details = createItemDetails(inventory[selectedItemIndex]);
    }
    
    // Combine all vertices
    vertices.insert(vertices.end(), background.begin(), background.end());
    vertices.insert(vertices.end(), slots.begin(), slots.end());
    vertices.insert(vertices.end(), icons.begin(), icons.end());
    vertices.insert(vertices.end(), highlight.begin(), highlight.end());
    vertices.insert(vertices.end(), details.begin(), details.end());
    
    return vertices;
}

bool InventoryUI::handleInput(int key, const std::shared_ptr<Character>& player) {
    if (!isVisible) {
        return false;
    }
    
    const auto& inventory = player->getInventory();
    if (inventory.empty()) {
        return false;
    }
    
    // Navigation keys
    switch (key) {
        case GLFW_KEY_UP:
            if (selectedItemIndex >= INVENTORY_COLS) {
                selectedItemIndex -= INVENTORY_COLS;
            }
            return true;
            
        case GLFW_KEY_DOWN:
            if (selectedItemIndex + INVENTORY_COLS < static_cast<int>(inventory.size())) {
                selectedItemIndex += INVENTORY_COLS;
            }
            return true;
            
        case GLFW_KEY_LEFT:
            if (selectedItemIndex > 0) {
                selectedItemIndex--;
            }
            return true;
            
        case GLFW_KEY_RIGHT:
            if (selectedItemIndex < static_cast<int>(inventory.size()) - 1) {
                selectedItemIndex++;
            }
            return true;
            
        case GLFW_KEY_E:
            useSelectedItem(player);
            return true;
            
        case GLFW_KEY_Q:
            dropSelectedItem(player);
            return true;
            
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_I:
            toggleVisibility();
            return true;
    }
    
    return false;
}

void InventoryUI::selectItem(int index) {
    selectedItemIndex = index;
}

void InventoryUI::useSelectedItem(const std::shared_ptr<Character>& player) {
    const auto& inventory = player->getInventory();
    if (inventory.empty() || selectedItemIndex < 0 || selectedItemIndex >= static_cast<int>(inventory.size())) {
        return;
    }
    
    // Use the selected item
    auto item = inventory[selectedItemIndex];
    std::cout << "Using item: " << item->getName() << std::endl;
    
    // Different behavior based on item type
    if (auto potion = std::dynamic_pointer_cast<Potion>(item)) {
        // Use potion to heal
        player->heal(potion->getHealAmount());
        player->removeItem(selectedItemIndex);
        
        // Adjust selected index if needed
        if (selectedItemIndex >= static_cast<int>(player->getInventory().size())) {
            selectedItemIndex = std::max(0, static_cast<int>(player->getInventory().size()) - 1);
        }
    } else if (auto weapon = std::dynamic_pointer_cast<Weapon>(item)) {
        // Equip weapon
        player->equipWeapon(weapon);
    } else if (auto armor = std::dynamic_pointer_cast<Armor>(item)) {
        // Equip armor
        player->equipArmor(armor);
    }
}

void InventoryUI::dropSelectedItem(const std::shared_ptr<Character>& player) {
    const auto& inventory = player->getInventory();
    if (inventory.empty() || selectedItemIndex < 0 || selectedItemIndex >= static_cast<int>(inventory.size())) {
        return;
    }
    
    // Drop the selected item
    auto item = inventory[selectedItemIndex];
    std::cout << "Dropping item: " << item->getName() << std::endl;
    
    // TODO: Create an item drop at the player's location
    
    // Remove the item from inventory
    player->removeItem(selectedItemIndex);
    
    // Adjust selected index if needed
    if (selectedItemIndex >= static_cast<int>(player->getInventory().size())) {
        selectedItemIndex = std::max(0, static_cast<int>(player->getInventory().size()) - 1);
    }
}

std::vector<Vertex> InventoryUI::createInventoryBackground() const {
    std::vector<Vertex> vertices;
    
    // Create a semi-transparent dark background
    float x = INVENTORY_X - INVENTORY_WIDTH / 2.0f;
    float y = INVENTORY_Y - INVENTORY_HEIGHT / 2.0f;
    
    // Background color (dark blue with transparency)
    float r = 0.1f, g = 0.1f, b = 0.3f;
    
    // Create rectangle vertices
    Vertex v1 = {{x, y}, {r, g, b}};
    Vertex v2 = {{x + INVENTORY_WIDTH, y}, {r, g, b}};
    Vertex v3 = {{x, y + INVENTORY_HEIGHT}, {r, g, b}};
    Vertex v4 = {{x + INVENTORY_WIDTH, y + INVENTORY_HEIGHT}, {r, g, b}};
    
    // First triangle
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    
    // Second triangle
    vertices.push_back(v2);
    vertices.push_back(v4);
    vertices.push_back(v3);
    
    // Add a border
    float borderWidth = 0.005f;
    float borderR = 0.5f, borderG = 0.5f, borderB = 0.8f;
    
    // Top border
    Vertex b1 = {{x - borderWidth, y - borderWidth}, {borderR, borderG, borderB}};
    Vertex b2 = {{x + INVENTORY_WIDTH + borderWidth, y - borderWidth}, {borderR, borderG, borderB}};
    Vertex b3 = {{x - borderWidth, y}, {borderR, borderG, borderB}};
    Vertex b4 = {{x + INVENTORY_WIDTH + borderWidth, y}, {borderR, borderG, borderB}};
    
    // Bottom border
    Vertex b5 = {{x - borderWidth, y + INVENTORY_HEIGHT}, {borderR, borderG, borderB}};
    Vertex b6 = {{x + INVENTORY_WIDTH + borderWidth, y + INVENTORY_HEIGHT}, {borderR, borderG, borderB}};
    Vertex b7 = {{x - borderWidth, y + INVENTORY_HEIGHT + borderWidth}, {borderR, borderG, borderB}};
    Vertex b8 = {{x + INVENTORY_WIDTH + borderWidth, y + INVENTORY_HEIGHT + borderWidth}, {borderR, borderG, borderB}};
    
    // Left border
    Vertex b9 = {{x - borderWidth, y}, {borderR, borderG, borderB}};
    Vertex b10 = {{x, y}, {borderR, borderG, borderB}};
    Vertex b11 = {{x - borderWidth, y + INVENTORY_HEIGHT}, {borderR, borderG, borderB}};
    Vertex b12 = {{x, y + INVENTORY_HEIGHT}, {borderR, borderG, borderB}};
    
    // Right border
    Vertex b13 = {{x + INVENTORY_WIDTH, y}, {borderR, borderG, borderB}};
    Vertex b14 = {{x + INVENTORY_WIDTH + borderWidth, y}, {borderR, borderG, borderB}};
    Vertex b15 = {{x + INVENTORY_WIDTH, y + INVENTORY_HEIGHT}, {borderR, borderG, borderB}};
    Vertex b16 = {{x + INVENTORY_WIDTH + borderWidth, y + INVENTORY_HEIGHT}, {borderR, borderG, borderB}};
    
    // Add border triangles
    vertices.push_back(b1); vertices.push_back(b2); vertices.push_back(b3);
    vertices.push_back(b2); vertices.push_back(b4); vertices.push_back(b3);
    
    vertices.push_back(b5); vertices.push_back(b6); vertices.push_back(b7);
    vertices.push_back(b6); vertices.push_back(b8); vertices.push_back(b7);
    
    vertices.push_back(b9); vertices.push_back(b10); vertices.push_back(b11);
    vertices.push_back(b10); vertices.push_back(b12); vertices.push_back(b11);
    
    vertices.push_back(b13); vertices.push_back(b14); vertices.push_back(b15);
    vertices.push_back(b14); vertices.push_back(b16); vertices.push_back(b15);
    
    // Add title
    float titleX = x + 0.1f;
    float titleY = y + INVENTORY_HEIGHT - 0.1f;
    float titleWidth = 0.4f;
    float titleHeight = 0.08f;
    float titleR = 0.9f, titleG = 0.9f, titleB = 0.9f;
    
    Vertex t1 = {{titleX, titleY}, {titleR, titleG, titleB}};
    Vertex t2 = {{titleX + titleWidth, titleY}, {titleR, titleG, titleB}};
    Vertex t3 = {{titleX, titleY + titleHeight}, {titleR, titleG, titleB}};
    Vertex t4 = {{titleX + titleWidth, titleY + titleHeight}, {titleR, titleG, titleB}};
    
    vertices.push_back(t1); vertices.push_back(t2); vertices.push_back(t3);
    vertices.push_back(t2); vertices.push_back(t4); vertices.push_back(t3);
    
    return vertices;
}

std::vector<Vertex> InventoryUI::createItemSlots() const {
    std::vector<Vertex> vertices;
    
    // Calculate starting position for the grid
    float startX = INVENTORY_X - ((INVENTORY_COLS * ITEM_SLOT_SIZE) + ((INVENTORY_COLS - 1) * ITEM_SLOT_PADDING)) / 2.0f;
    float startY = INVENTORY_Y - ((INVENTORY_ROWS * ITEM_SLOT_SIZE) + ((INVENTORY_ROWS - 1) * ITEM_SLOT_PADDING)) / 2.0f;
    
    // Slot color (slightly lighter than background)
    float r = 0.2f, g = 0.2f, b = 0.4f;
    
    // Create slots
    for (int row = 0; row < INVENTORY_ROWS; row++) {
        for (int col = 0; col < INVENTORY_COLS; col++) {
            float x = startX + col * (ITEM_SLOT_SIZE + ITEM_SLOT_PADDING);
            float y = startY + row * (ITEM_SLOT_SIZE + ITEM_SLOT_PADDING);
            
            // Create rectangle vertices
            Vertex v1 = {{x, y}, {r, g, b}};
            Vertex v2 = {{x + ITEM_SLOT_SIZE, y}, {r, g, b}};
            Vertex v3 = {{x, y + ITEM_SLOT_SIZE}, {r, g, b}};
            Vertex v4 = {{x + ITEM_SLOT_SIZE, y + ITEM_SLOT_SIZE}, {r, g, b}};
            
            // First triangle
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
            // Second triangle
            vertices.push_back(v2);
            vertices.push_back(v4);
            vertices.push_back(v3);
        }
    }
    
    return vertices;
}

std::vector<Vertex> InventoryUI::createItemIcons(const std::vector<std::shared_ptr<Item>>& items) const {
    std::vector<Vertex> vertices;
    
    // Calculate starting position for the grid
    float startX = INVENTORY_X - ((INVENTORY_COLS * ITEM_SLOT_SIZE) + ((INVENTORY_COLS - 1) * ITEM_SLOT_PADDING)) / 2.0f;
    float startY = INVENTORY_Y - ((INVENTORY_ROWS * ITEM_SLOT_SIZE) + ((INVENTORY_ROWS - 1) * ITEM_SLOT_PADDING)) / 2.0f;
    
    // Create item icons
    for (size_t i = 0; i < items.size() && i < static_cast<size_t>(INVENTORY_ROWS * INVENTORY_COLS); i++) {
        int row = i / INVENTORY_COLS;
        int col = i % INVENTORY_COLS;
        
        float x = startX + col * (ITEM_SLOT_SIZE + ITEM_SLOT_PADDING) + 0.01f;
        float y = startY + row * (ITEM_SLOT_SIZE + ITEM_SLOT_PADDING) + 0.01f;
        float size = ITEM_SLOT_SIZE - 0.02f;
        
        // Get item color based on rarity
        float r, g, b;
        getItemColor(items[i]->getRarity(), r, g, b);
        
        // Create icon shape based on item type
        if (std::dynamic_pointer_cast<Weapon>(items[i])) {
            // Weapon icon (sword shape)
            Vertex v1 = {{x + size/2, y}, {r, g, b}};
            Vertex v2 = {{x + size, y + size/2}, {r, g, b}};
            Vertex v3 = {{x + size/2, y + size}, {r, g, b}};
            Vertex v4 = {{x, y + size/2}, {r, g, b}};
            
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
            vertices.push_back(v3);
            vertices.push_back(v4);
            vertices.push_back(v1);
        }
        else if (std::dynamic_pointer_cast<Armor>(items[i])) {
            // Armor icon (shield shape)
            Vertex v1 = {{x + size/2, y}, {r, g, b}};
            Vertex v2 = {{x + size, y + size/4}, {r, g, b}};
            Vertex v3 = {{x + size, y + size*3/4}, {r, g, b}};
            Vertex v4 = {{x + size/2, y + size}, {r, g, b}};
            Vertex v5 = {{x, y + size*3/4}, {r, g, b}};
            Vertex v6 = {{x, y + size/4}, {r, g, b}};
            
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v6);
            
            vertices.push_back(v2);
            vertices.push_back(v3);
            vertices.push_back(v5);
            
            vertices.push_back(v3);
            vertices.push_back(v4);
            vertices.push_back(v5);
            
            vertices.push_back(v2);
            vertices.push_back(v5);
            vertices.push_back(v6);
        }
        else if (std::dynamic_pointer_cast<Potion>(items[i])) {
            // Potion icon (circle shape)
            const int segments = 12;
            float centerX = x + size/2;
            float centerY = y + size/2;
            float radius = size/2 - 0.01f;
            
            for (int j = 0; j < segments; j++) {
                float angle1 = 2.0f * 3.14159f * j / segments;
                float angle2 = 2.0f * 3.14159f * (j+1) / segments;
                
                Vertex v1 = {{centerX, centerY}, {r, g, b}};
                Vertex v2 = {{centerX + radius * cos(angle1), centerY + radius * sin(angle1)}, {r, g, b}};
                Vertex v3 = {{centerX + radius * cos(angle2), centerY + radius * sin(angle2)}, {r, g, b}};
                
                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v3);
            }
        }
        else {
            // Generic item icon (square)
            Vertex v1 = {{x, y}, {r, g, b}};
            Vertex v2 = {{x + size, y}, {r, g, b}};
            Vertex v3 = {{x, y + size}, {r, g, b}};
            Vertex v4 = {{x + size, y + size}, {r, g, b}};
            
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
            vertices.push_back(v2);
            vertices.push_back(v4);
            vertices.push_back(v3);
        }
    }
    
    return vertices;
}

std::vector<Vertex> InventoryUI::createSelectionHighlight() const {
    std::vector<Vertex> vertices;
    
    const auto& inventory = std::vector<std::shared_ptr<Item>>(); // Placeholder, we'll get this from the player
    if (inventory.empty() || selectedItemIndex < 0 || selectedItemIndex >= static_cast<int>(inventory.size())) {
        return vertices;
    }
    
    // Calculate position of the selected item
    int row = selectedItemIndex / INVENTORY_COLS;
    int col = selectedItemIndex % INVENTORY_COLS;
    
    // Calculate starting position for the grid
    float startX = INVENTORY_X - ((INVENTORY_COLS * ITEM_SLOT_SIZE) + ((INVENTORY_COLS - 1) * ITEM_SLOT_PADDING)) / 2.0f;
    float startY = INVENTORY_Y - ((INVENTORY_ROWS * ITEM_SLOT_SIZE) + ((INVENTORY_ROWS - 1) * ITEM_SLOT_PADDING)) / 2.0f;
    
    float x = startX + col * (ITEM_SLOT_SIZE + ITEM_SLOT_PADDING);
    float y = startY + row * (ITEM_SLOT_SIZE + ITEM_SLOT_PADDING);
    
    // Add a pulsing effect using the animation timer
    float pulseScale = 1.0f + 0.05f * std::sin(animationTimer * 4.0f);
    float borderWidth = 0.005f * pulseScale;
    
    // Highlight color (bright yellow)
    float r = 1.0f, g = 1.0f, b = 0.0f;
    
    // Top border
    Vertex v1 = {{x - borderWidth, y - borderWidth}, {r, g, b}};
    Vertex v2 = {{x + ITEM_SLOT_SIZE + borderWidth, y - borderWidth}, {r, g, b}};
    Vertex v3 = {{x - borderWidth, y}, {r, g, b}};
    Vertex v4 = {{x + ITEM_SLOT_SIZE + borderWidth, y}, {r, g, b}};
    
    // Bottom border
    Vertex v5 = {{x - borderWidth, y + ITEM_SLOT_SIZE}, {r, g, b}};
    Vertex v6 = {{x + ITEM_SLOT_SIZE + borderWidth, y + ITEM_SLOT_SIZE}, {r, g, b}};
    Vertex v7 = {{x - borderWidth, y + ITEM_SLOT_SIZE + borderWidth}, {r, g, b}};
    Vertex v8 = {{x + ITEM_SLOT_SIZE + borderWidth, y + ITEM_SLOT_SIZE + borderWidth}, {r, g, b}};
    
    // Left border
    Vertex v9 = {{x - borderWidth, y}, {r, g, b}};
    Vertex v10 = {{x, y}, {r, g, b}};
    Vertex v11 = {{x - borderWidth, y + ITEM_SLOT_SIZE}, {r, g, b}};
    Vertex v12 = {{x, y + ITEM_SLOT_SIZE}, {r, g, b}};
    
    // Right border
    Vertex v13 = {{x + ITEM_SLOT_SIZE, y}, {r, g, b}};
    Vertex v14 = {{x + ITEM_SLOT_SIZE + borderWidth, y}, {r, g, b}};
    Vertex v15 = {{x + ITEM_SLOT_SIZE, y + ITEM_SLOT_SIZE}, {r, g, b}};
    Vertex v16 = {{x + ITEM_SLOT_SIZE + borderWidth, y + ITEM_SLOT_SIZE}, {r, g, b}};
    
    // Add all border triangles
    vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
    vertices.push_back(v2); vertices.push_back(v4); vertices.push_back(v3);
    
    vertices.push_back(v5); vertices.push_back(v6); vertices.push_back(v7);
    vertices.push_back(v6); vertices.push_back(v8); vertices.push_back(v7);
    
    vertices.push_back(v9); vertices.push_back(v10); vertices.push_back(v11);
    vertices.push_back(v10); vertices.push_back(v12); vertices.push_back(v11);
    
    vertices.push_back(v13); vertices.push_back(v14); vertices.push_back(v15);
    vertices.push_back(v14); vertices.push_back(v16); vertices.push_back(v15);
    
    return vertices;
}

std::vector<Vertex> InventoryUI::createItemDetails(const std::shared_ptr<Item>& item) const {
    std::vector<Vertex> vertices;
    
    // Create a box for item details
    float x = INVENTORY_X - INVENTORY_WIDTH / 2.0f + 0.05f;
    float y = INVENTORY_Y + INVENTORY_HEIGHT / 2.0f - 0.25f;
    float width = INVENTORY_WIDTH - 0.1f;
    float height = 0.2f;
    
    // Background color (dark gray with some transparency)
    float r = 0.2f, g = 0.2f, b = 0.2f;
    
    // Create rectangle vertices
    Vertex v1 = {{x, y}, {r, g, b}};
    Vertex v2 = {{x + width, y}, {r, g, b}};
    Vertex v3 = {{x, y + height}, {r, g, b}};
    Vertex v4 = {{x + width, y + height}, {r, g, b}};
    
    // First triangle
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    
    // Second triangle
    vertices.push_back(v2);
    vertices.push_back(v4);
    vertices.push_back(v3);
    
    // Add item name with color based on rarity
    float nameX = x + 0.02f;
    float nameY = y + height - 0.05f;
    float nameWidth = width - 0.04f;
    float nameHeight = 0.04f;
    
    float nameR, nameG, nameB;
    getItemColor(item->getRarity(), nameR, nameG, nameB);
    
    Vertex n1 = {{nameX, nameY}, {nameR, nameG, nameB}};
    Vertex n2 = {{nameX + nameWidth, nameY}, {nameR, nameG, nameB}};
    Vertex n3 = {{nameX, nameY + nameHeight}, {nameR, nameG, nameB}};
    Vertex n4 = {{nameX + nameWidth, nameY + nameHeight}, {nameR, nameG, nameB}};
    
    vertices.push_back(n1);
    vertices.push_back(n2);
    vertices.push_back(n3);
    
    vertices.push_back(n2);
    vertices.push_back(n4);
    vertices.push_back(n3);
    
    // Add item description
    float descX = x + 0.02f;
    float descY = y + 0.02f;
    float descWidth = width - 0.04f;
    float descHeight = height - 0.07f;
    
    // Description text color (light gray)
    float descR = 0.8f, descG = 0.8f, descB = 0.8f;
    
    Vertex d1 = {{descX, descY}, {descR, descG, descB}};
    Vertex d2 = {{descX + descWidth, descY}, {descR, descG, descB}};
    Vertex d3 = {{descX, descY + descHeight}, {descR, descG, descB}};
    Vertex d4 = {{descX + descWidth, descY + descHeight}, {descR, descG, descB}};
    
    vertices.push_back(d1);
    vertices.push_back(d2);
    vertices.push_back(d3);
    
    vertices.push_back(d2);
    vertices.push_back(d4);
    vertices.push_back(d3);
    
    return vertices;
}

void InventoryUI::getItemColor(ItemRarity rarity, float& r, float& g, float& b) const {
    switch (rarity) {
        case ItemRarity::COMMON:
            r = 0.7f; g = 0.7f; b = 0.7f; // Gray
            break;
        case ItemRarity::UNCOMMON:
            r = 0.0f; g = 0.8f; b = 0.0f; // Green
            break;
        case ItemRarity::RARE:
            r = 0.0f; g = 0.0f; b = 0.8f; // Blue
            break;
        case ItemRarity::EPIC:
            r = 0.8f; g = 0.0f; b = 0.8f; // Purple
            break;
        case ItemRarity::LEGENDARY:
            r = 1.0f; g = 0.6f; b = 0.0f; // Orange
            break;
        default:
            r = 1.0f; g = 1.0f; b = 1.0f; // White
    }
}
