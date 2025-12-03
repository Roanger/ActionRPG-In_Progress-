#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vulkan_renderer.h"
#include "renderer.h"
#include "../game/character.h"
#include "../game/level.h"
#include "../game/visual_effect.h"
#include "../ui/character_select.h"
#include "../ui/inventory_ui.h"
#include "../game/item.h"
#include "../engine/renderer.h"
#include "../ui/ui_system.h"
#include <map>
#include <memory>

// Game states
enum class GameState {
    CHARACTER_SELECT,
    PLAYING,
    GAME_OVER,
    PAUSE_MENU
};

class GameLoop {
public:
    GameLoop(VulkanRenderer* renderer);
    ~GameLoop();
    
    void run();
    
private:
    VulkanRenderer* vulkanRenderer;
    std::unique_ptr<Renderer> renderer;
    GLFWwindow* window;
    
    // Game state management
    GameState currentState;
    std::unique_ptr<CharacterSelectScreen> characterSelectScreen;
    std::unique_ptr<InventoryUI> inventoryUI;
    std::unique_ptr<UISystem> uiSystem;
    
    // Game objects
    std::shared_ptr<Character> player;
    std::shared_ptr<Level> currentLevel;
    VisualEffectManager effectManager;
    
    bool initialize();
    void cleanup();
    void processInput();
    void update(float deltaTime);
    void render();
    
    // Game state methods
    void updateCharacterSelect(float deltaTime);
    void renderCharacterSelect();
    void handleCharacterSelectInput();
    void createPlayerCharacter(CharacterClass characterClass);
    void startGame();
    
    // Input handling
    void handlePlayerMovement(float deltaTime);
    void handlePlayerActions();
    // Helper for input state
    std::map<int, bool> previousKeyStates;
    bool isKeyJustPressed(int key);
    
    const int WIDTH = 800;
    const int HEIGHT = 600;
    const char* WINDOW_TITLE = "Action RPG";
    
    // Constants
    const float PLAYER_SPEED = 1.0f; // Reduced for grid-based movement
    const float MOVEMENT_COOLDOWN = 0.1f; // Reduced for more responsive controls
    
    // Movement state
    float movementTimer = 0.0f;
}; 