#pragma once

#include <vector>
#include <memory>
#include <string>
#include "../engine/vertex.h"
#include "../game/character.h"
#include "../game/item.h"

// Class to handle the inventory UI display and interaction
class InventoryUI {
public:
    InventoryUI();
    ~InventoryUI();
    
    // Toggle inventory visibility
    void toggleVisibility() { isVisible = !isVisible; }
    bool isInventoryVisible() const { return isVisible; }
    
    // Update the inventory UI state
    void update(float deltaTime);
    
    // Generate vertices for rendering the inventory UI
    std::vector<Vertex> generateInventoryVertices(const std::shared_ptr<Character>& player) const;
    
    // Handle input for inventory navigation and item usage
    bool handleInput(int key, const std::shared_ptr<Character>& player);
    
    // Select an item in the inventory
    void selectItem(int index);
    
    // Use the currently selected item
    void useSelectedItem(const std::shared_ptr<Character>& player);
    
    // Drop the currently selected item
    void dropSelectedItem(const std::shared_ptr<Character>& player);
    
private:
    bool isVisible;
    int selectedItemIndex;
    float animationTimer;
    
    // Constants for UI layout
    static constexpr float INVENTORY_WIDTH = 1.2f;
    static constexpr float INVENTORY_HEIGHT = 1.6f;
    static constexpr float ITEM_SLOT_SIZE = 0.15f;
    static constexpr float ITEM_SLOT_PADDING = 0.03f;
    static constexpr float INVENTORY_X = 0.0f;  // Centered
    static constexpr float INVENTORY_Y = 0.0f;  // Centered
    static constexpr int INVENTORY_COLS = 5;
    static constexpr int INVENTORY_ROWS = 4;
    
    // Helper methods for rendering
    std::vector<Vertex> createInventoryBackground() const;
    std::vector<Vertex> createItemSlots() const;
    std::vector<Vertex> createItemIcons(const std::vector<std::shared_ptr<Item>>& items) const;
    std::vector<Vertex> createSelectionHighlight() const;
    std::vector<Vertex> createItemDetails(const std::shared_ptr<Item>& item) const;
    
    // Helper method to get item color based on rarity
    void getItemColor(ItemRarity rarity, float& r, float& g, float& b) const;
};
