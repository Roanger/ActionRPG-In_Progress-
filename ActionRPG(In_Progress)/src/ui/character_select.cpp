#include "character_select.h"
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>

CharacterSelectScreen::CharacterSelectScreen(UISystem* uiSystem)
    : selectedIndex(0), selectionConfirmed(false), animationTimer(0.0f), uiSystem(uiSystem) {
}

CharacterSelectScreen::~CharacterSelectScreen() {
}

void CharacterSelectScreen::update(float deltaTime) {
    // Update animation timer
    animationTimer += deltaTime;
    if (animationTimer > 6.28318f) { // 2*PI
        animationTimer -= 6.28318f;
    }
}

std::vector<Vertex> CharacterSelectScreen::generateVertices(UISystem* uiSystem) const {
    std::vector<Vertex> vertices;
    
    // Combine all UI elements
    auto background = createBackground();
    auto title = createTitle();
    auto options = createCharacterOptions();
    auto highlight = createSelectionHighlight();
    auto details = createCharacterDetails();
    auto instructions = createInstructions();
    
    vertices.insert(vertices.end(), background.begin(), background.end());
    vertices.insert(vertices.end(), title.begin(), title.end());
    vertices.insert(vertices.end(), options.begin(), options.end());
    vertices.insert(vertices.end(), highlight.begin(), highlight.end());
    vertices.insert(vertices.end(), details.begin(), details.end());
    vertices.insert(vertices.end(), instructions.begin(), instructions.end());
    
    return vertices;
}

bool CharacterSelectScreen::handleInput(int key) {
    // Left/Right to navigate between character options
    if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) {
        selectedIndex = (selectedIndex + NUM_CLASSES - 1) % NUM_CLASSES;
        return true;
    }
    else if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) {
        selectedIndex = (selectedIndex + 1) % NUM_CLASSES;
        return true;
    }
    // Enter/Space to confirm selection
    else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) {
        selectionConfirmed = true;
        std::cout << "Selected character class: " << classNames[selectedIndex] << std::endl;
        return true;
    }
    
    return false;
}

CharacterClass CharacterSelectScreen::getSelectedClass() const {
    return characterClasses[selectedIndex];
}

std::vector<Vertex> CharacterSelectScreen::createBackground() const {
    std::vector<Vertex> vertices;
    
    // Semi-transparent dark background
    float x = -SCREEN_WIDTH / 2.0f;
    float y = -SCREEN_HEIGHT / 2.0f;
    float width = SCREEN_WIDTH;
    float height = SCREEN_HEIGHT;
    
    // Background color (dark blue)
    float r = 0.1f, g = 0.1f, b = 0.3f;
    
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
    
    return vertices;
}

std::vector<Vertex> CharacterSelectScreen::createTitle() const {
    // Create the title text using the UI System
    if (!uiSystem) return {}; // Return empty if uiSystem is not initialized
    return uiSystem->createStatusText(
        -0.5f, -SCREEN_HEIGHT / 2.0f + 0.1f,
        "Select Your Character"
    );
}

std::vector<Vertex> CharacterSelectScreen::createCharacterOptions() const {
    std::vector<Vertex> vertices;
    
    if (!uiSystem) return vertices; // Return empty if uiSystem is not initialized
    
    // Create character option boxes
    float startX = -((NUM_CLASSES * OPTION_WIDTH) + ((NUM_CLASSES - 1) * OPTION_SPACING)) / 2.0f;
    float y = -0.2f;
    
    for (int i = 0; i < NUM_CLASSES; i++) {
        float x = startX + i * (OPTION_WIDTH + OPTION_SPACING);
        
        // Character class colors
        float r, g, b;
        switch (characterClasses[i]) {
            case CharacterClass::WARRIOR:
                r = 0.8f; g = 0.2f; b = 0.2f; // Red
                break;
            case CharacterClass::RANGER:
                r = 0.2f; g = 0.8f; b = 0.2f; // Green
                break;
            case CharacterClass::MAGE:
                r = 0.2f; g = 0.2f; b = 0.8f; // Blue
                break;
            default:
                r = 0.7f; g = 0.7f; b = 0.7f; // Gray
        }
        
        // Create character option box
        Vertex v1 = {{x, y}, {r, g, b}};
        Vertex v2 = {{x + OPTION_WIDTH, y}, {r, g, b}};
        Vertex v3 = {{x, y + OPTION_HEIGHT}, {r, g, b}};
        Vertex v4 = {{x + OPTION_WIDTH, y + OPTION_HEIGHT}, {r, g, b}};
        
        // First triangle
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        
        // Second triangle
        vertices.push_back(v2);
        vertices.push_back(v4);
        vertices.push_back(v3);
        
        // Add character name text using the UI System
        auto nameText = uiSystem->createStatusText(
            x + 0.05f, y + OPTION_HEIGHT - 0.1f,
            classNames[i] // Use the actual class name
        );
        
        vertices.insert(vertices.end(), nameText.begin(), nameText.end());
    }
    
    return vertices;
}

std::vector<Vertex> CharacterSelectScreen::createSelectionHighlight() const {
    std::vector<Vertex> vertices;
    
    // Calculate position of the selected character option
    float startX = -((NUM_CLASSES * OPTION_WIDTH) + ((NUM_CLASSES - 1) * OPTION_SPACING)) / 2.0f;
    float x = startX + selectedIndex * (OPTION_WIDTH + OPTION_SPACING);
    float y = -0.2f;
    
    // Add a pulsing effect using the animation timer
    float pulseScale = 1.0f + 0.05f * std::sin(animationTimer * 4.0f);
    float borderWidth = 0.01f * pulseScale;
    
    // Highlight color (bright yellow)
    float r = 1.0f, g = 1.0f, b = 0.0f;
    
    // Top border
    Vertex v1 = {{x - borderWidth, y - borderWidth}, {r, g, b}};
    Vertex v2 = {{x + OPTION_WIDTH + borderWidth, y - borderWidth}, {r, g, b}};
    Vertex v3 = {{x - borderWidth, y}, {r, g, b}};
    Vertex v4 = {{x + OPTION_WIDTH + borderWidth, y}, {r, g, b}};
    
    // Bottom border
    Vertex v5 = {{x - borderWidth, y + OPTION_HEIGHT}, {r, g, b}};
    Vertex v6 = {{x + OPTION_WIDTH + borderWidth, y + OPTION_HEIGHT}, {r, g, b}};
    Vertex v7 = {{x - borderWidth, y + OPTION_HEIGHT + borderWidth}, {r, g, b}};
    Vertex v8 = {{x + OPTION_WIDTH + borderWidth, y + OPTION_HEIGHT + borderWidth}, {r, g, b}};
    
    // Left border
    Vertex v9 = {{x - borderWidth, y}, {r, g, b}};
    Vertex v10 = {{x, y}, {r, g, b}};
    Vertex v11 = {{x - borderWidth, y + OPTION_HEIGHT}, {r, g, b}};
    Vertex v12 = {{x, y + OPTION_HEIGHT}, {r, g, b}};
    
    // Right border
    Vertex v13 = {{x + OPTION_WIDTH, y}, {r, g, b}};
    Vertex v14 = {{x + OPTION_WIDTH + borderWidth, y}, {r, g, b}};
    Vertex v15 = {{x + OPTION_WIDTH, y + OPTION_HEIGHT}, {r, g, b}};
    Vertex v16 = {{x + OPTION_WIDTH + borderWidth, y + OPTION_HEIGHT}, {r, g, b}};
    
    // Add all border triangles
    // Top
    vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
    vertices.push_back(v2); vertices.push_back(v4); vertices.push_back(v3);
    
    // Bottom
    vertices.push_back(v5); vertices.push_back(v6); vertices.push_back(v7);
    vertices.push_back(v6); vertices.push_back(v8); vertices.push_back(v7);
    
    // Left
    vertices.push_back(v9); vertices.push_back(v10); vertices.push_back(v11);
    vertices.push_back(v10); vertices.push_back(v12); vertices.push_back(v11);
    
    // Right
    vertices.push_back(v13); vertices.push_back(v14); vertices.push_back(v15);
    vertices.push_back(v14); vertices.push_back(v16); vertices.push_back(v15);
    
    return vertices;
}

std::vector<Vertex> CharacterSelectScreen::createCharacterDetails() const {
    std::vector<Vertex> vertices;
    
    if (!uiSystem) return vertices; // Return empty if uiSystem is not initialized

    // Create a box for character details
    float x = -0.7f;
    float y = 0.1f;
    float width = 1.4f;
    float height = 0.4f;
    
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
    
    // Add description text using the UI System
    // Use a placeholder description for now, ideally this would come from character data
    std::string description = "Details for " + classNames[selectedIndex] + "\n" + "[Placeholder Description]";
    auto descText = uiSystem->createStatusText(
        x + 0.05f, y + 0.05f,
        description
    );
    
    vertices.insert(vertices.end(), descText.begin(), descText.end());
    
    return vertices;
}

std::vector<Vertex> CharacterSelectScreen::createInstructions() const {
    // Create the instructions text using the UI System
     if (!uiSystem) return {}; // Return empty if uiSystem is not initialized
    return uiSystem->createStatusText(
        -0.7f, SCREEN_HEIGHT / 2.0f - 0.15f,
        "Use A/D or Left/Right arrows to select, Enter or Space to confirm"
    );
}
