#pragma once

#include <vector>
#include <memory>
#include <string>
#include "../engine/vertex.h"
#include "../game/character.h"
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>

#include "ui_system.h"

// Class to handle the character selection screen
class CharacterSelectScreen {
public:
    CharacterSelectScreen(UISystem* uiSystem);
    ~CharacterSelectScreen();
    
    // Update the selection screen state
    void update(float deltaTime);
    
    // Generate vertices for rendering the character selection screen
    std::vector<Vertex> generateVertices(UISystem* uiSystem) const;
    
    // Handle input for character selection
    bool handleInput(int key);
    
    // Get the selected character class
    CharacterClass getSelectedClass() const;
    
    // Check if a character has been selected and confirmed
    bool isSelectionConfirmed() const { return selectionConfirmed; }
    
private:
    int selectedIndex;
    bool selectionConfirmed;
    float animationTimer;
    
    // Pointer to the UI System for text rendering
    UISystem* uiSystem;
    
    // Character class options
    static const int NUM_CLASSES = 3;
    CharacterClass characterClasses[NUM_CLASSES] = {
        CharacterClass::WARRIOR,
        CharacterClass::RANGER,
        CharacterClass::MAGE
    };
    
    std::string classNames[NUM_CLASSES] = {
        "Warrior",
        "Ranger",
        "Mage"
    };
    
    std::string classDescriptions[NUM_CLASSES] = {
        "Strong melee fighter with high health and strength. Uses swords and axes.",
        "Agile ranged fighter with balanced stats. Uses bows and daggers.",
        "Powerful spellcaster with high intelligence but low health. Uses magic spells."
    };
    
    // Constants for UI layout
    static constexpr float SCREEN_WIDTH = 1.8f;
    static constexpr float SCREEN_HEIGHT = 1.4f;
    static constexpr float OPTION_WIDTH = 0.4f;
    static constexpr float OPTION_HEIGHT = 0.7f;
    static constexpr float OPTION_SPACING = 0.1f;
    
    // Helper methods for rendering
    std::vector<Vertex> createBackground() const;
    std::vector<Vertex> createTitle() const;
    std::vector<Vertex> createCharacterOptions() const;
    std::vector<Vertex> createSelectionHighlight() const;
    std::vector<Vertex> createCharacterDetails() const;
    std::vector<Vertex> createInstructions() const;
    
    // Helper method to create text vertices (simplified for this implementation)
    std::vector<Vertex> createTextPlaceholder(float x, float y, float width, float height, float r, float g, float b) const;
};
