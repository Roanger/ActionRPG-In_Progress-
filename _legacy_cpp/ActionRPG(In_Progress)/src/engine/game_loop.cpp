#include "game_loop.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include "../game/enemy.h"
#include "../game/item.h"
#include "../engine/renderer.h"
#include "../ui/ui_system.h"

GameLoop::GameLoop(VulkanRenderer* vulkanRenderer) : vulkanRenderer(vulkanRenderer), window(nullptr), currentState(GameState::CHARACTER_SELECT) {
    // Initialize UI system
    uiSystem = std::make_unique<UISystem>();

    // Initialize character selection screen, passing the UI system
    characterSelectScreen = std::make_unique<CharacterSelectScreen>(uiSystem.get());
    
    // Initialize inventory UI
    inventoryUI = std::make_unique<InventoryUI>();
}

GameLoop::~GameLoop() {
    cleanup();
}

bool GameLoop::initialize() {
    try {
        std::cout << "Starting GameLoop initialization..." << std::endl;
        
        // Note: GLFW is already initialized in the VulkanRenderer class
        
        // Create window with Vulkan support
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        
        // Add this line to debug window creation
        std::cout << "Creating GLFW window..." << std::endl;
        
        window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        
        std::cout << "GLFW window created successfully" << std::endl;
        
        // Create Vulkan surface
        std::cout << "Creating Vulkan surface..." << std::endl;
        if (!vulkanRenderer->createSurface(window)) {
            std::cerr << "Failed to create Vulkan surface" << std::endl;
            return false;
        }
        
        std::cout << "Vulkan surface created successfully" << std::endl;
        
        // Initialize the Vulkan swapchain and rendering components
        std::cout << "Initializing Vulkan swapchain..." << std::endl;
        if (!vulkanRenderer->initializeSwapchain()) {
            std::cerr << "Failed to initialize Vulkan swapchain" << std::endl;
            return false;
        }
        std::cout << "Vulkan swapchain initialized successfully" << std::endl;
        
        // Note: We no longer create the player and level here
        // They will be created after character selection
        std::cout << "Game will start with character selection screen" << std::endl;
        
        // Create renderer
        std::cout << "Creating renderer..." << std::endl;
        renderer = std::make_unique<Renderer>(vulkanRenderer);
        if (!renderer) {
            std::cerr << "Failed to create renderer" << std::endl;
            return false;
        }
        
        std::cout << "Initializing renderer..." << std::endl;
        renderer->initialize();
        std::cout << "Renderer initialized successfully" << std::endl;

        vulkanRenderer->setRenderer(renderer.get());

        std::cout << "GameLoop initialization complete" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in GameLoop::initialize: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown exception in GameLoop::initialize" << std::endl;
        return false;
    }
}

void GameLoop::cleanup() {
    // First, clean up the renderer resources
    if (renderer) {
        // Explicitly clean up renderer resources before it's destroyed
        renderer->cleanupResources();
        // Reset the unique_ptr to destroy the Renderer
        renderer.reset();
    }
    
    // Then clean up the window
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    
    // Finally terminate GLFW
    glfwTerminate();
}

void GameLoop::processInput() {
    // Check if escape key is pressed to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    // Handle input based on current game state
    switch (currentState) {
        case GameState::CHARACTER_SELECT:
            handleCharacterSelectInput();
            break;
            
        case GameState::PLAYING:
            // Check for inventory toggle
            if (isKeyJustPressed(GLFW_KEY_I)) {
                inventoryUI->toggleVisibility();
                std::cout << "Inventory toggled: " << (inventoryUI->isInventoryVisible() ? "visible" : "hidden") << std::endl;
            }
            
            // Handle inventory input if visible
            if (inventoryUI->isInventoryVisible() && player) {
                // Handle inventory navigation keys
                if (isKeyJustPressed(GLFW_KEY_UP) || isKeyJustPressed(GLFW_KEY_DOWN) || 
                    isKeyJustPressed(GLFW_KEY_LEFT) || isKeyJustPressed(GLFW_KEY_RIGHT) || 
                    isKeyJustPressed(GLFW_KEY_E) || isKeyJustPressed(GLFW_KEY_Q)) {
                    
                    int key = -1;
                    if (isKeyJustPressed(GLFW_KEY_UP)) key = GLFW_KEY_UP;
                    else if (isKeyJustPressed(GLFW_KEY_DOWN)) key = GLFW_KEY_DOWN;
                    else if (isKeyJustPressed(GLFW_KEY_LEFT)) key = GLFW_KEY_LEFT;
                    else if (isKeyJustPressed(GLFW_KEY_RIGHT)) key = GLFW_KEY_RIGHT;
                    else if (isKeyJustPressed(GLFW_KEY_E)) key = GLFW_KEY_E;
                    else if (isKeyJustPressed(GLFW_KEY_Q)) key = GLFW_KEY_Q;
                    
                    if (key != -1) {
                        inventoryUI->handleInput(key, player);
                    }
                }
            }
            break;
            
        case GameState::GAME_OVER:
            // Check for restart key
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                // Reset to character selection
                currentState = GameState::CHARACTER_SELECT;
                characterSelectScreen = std::make_unique<CharacterSelectScreen>(uiSystem.get());
            }
            break;
            
        case GameState::PAUSE_MENU:
            // Check for unpause key
            if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
                currentState = GameState::PLAYING;
            }
            break;
    }
}

void GameLoop::handlePlayerMovement(float deltaTime) {
    // Update movement cooldown timer
    movementTimer -= deltaTime;
    
    // Only process movement if the cooldown timer has expired
    if (movementTimer <= 0.0f) {
        float dx = 0.0f;
        float dy = 0.0f;
        bool moved = false;
        
        // Get movement input - support both WASD and arrow keys
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            dy -= 1.0f;
            moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            dy += 1.0f;
            moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            dx -= 1.0f;
            moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            dx += 1.0f;
            moved = true;
        }
        
        // Only process movement if there was input
        if (moved) {
            // Normalize diagonal movement
            if (dx != 0.0f && dy != 0.0f) {
                float length = std::sqrt(dx * dx + dy * dy);
                dx /= length;
                dy /= length;
            }
            
            // Calculate new position
            float newX = player->getX() + dx;
            float newY = player->getY() + dy;
            
            // Check if new position is walkable
            int tileX = static_cast<int>(std::round(newX));
            int tileY = static_cast<int>(std::round(newY));
            
            if (currentLevel->isWalkable(tileX, tileY)) {
                // Move player to the new position
                player->move(newX, newY);
                
                // Reset movement cooldown
                movementTimer = MOVEMENT_COOLDOWN;
                
                // Debug output
                std::cout << "Player moved to: (" << player->getX() << ", " << player->getY() << ")" << std::endl;
            }
        }
    }
}

void GameLoop::handlePlayerActions() {
    static bool attackKeyPressed = false;
    
    // Attack nearest enemy
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!attackKeyPressed) {
            attackKeyPressed = true;
            
            // Find the nearest enemy
            auto enemies = currentLevel->getEnemies();
            std::shared_ptr<Enemy> nearestEnemy = nullptr;
            float minDistance = std::numeric_limits<float>::max();
            
            for (const auto& enemy : enemies) {
                if (enemy->isDead()) continue;
                
                float dx = enemy->getX() - player->getX();
                float dy = enemy->getY() - player->getY();
                float distanceSquared = dx * dx + dy * dy;
                
                if (distanceSquared < minDistance) {
                    minDistance = distanceSquared;
                    nearestEnemy = enemy;
                }
            }
            
            if (nearestEnemy) {
                // Check if enemy is in range based on player's attack type
                float distance = std::sqrt(minDistance);
                float attackRange = player->getAttackRange();
                
                if (distance <= attackRange) {
                    // Perform the attack
                    player->attack(nearestEnemy.get());
                    
                    // Create appropriate visual effect based on character class
                    Character::VisualEffectType effectType = player->getAttackVisualEffect();
                    
                    // For melee attacks, effect appears at enemy position
                    if (player->getDefaultAttackType() == Character::AttackType::MELEE) {
                        effectManager.addEffect(effectType, 
                                              player->getX(), player->getY(),
                                              nearestEnemy->getX(), nearestEnemy->getY());
                    }
                    // For ranged attacks, effect travels from player to enemy
                    else {
                        effectManager.addEffect(effectType, 
                                              player->getX(), player->getY(),
                                              nearestEnemy->getX(), nearestEnemy->getY());
                    }
                } else {
                    std::cout << "Enemy is out of range!" << std::endl;
                }
            }
        }
    } else {
        // Reset attack key state when key is released
        attackKeyPressed = false;
    }
    
    // Pick up item
    static bool pickupKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!pickupKeyPressed) {
            pickupKeyPressed = true;
            
            // Check for items in pickup range
            int itemIndex = currentLevel->getPickupItemIndex(player->getX(), player->getY());
            if (itemIndex >= 0) {
                // Pickup the item
                auto item = currentLevel->pickupItem(itemIndex);
                if (item) {
                    // Add to player inventory
                    player->addItem(item);
                    
                    // Display item information based on rarity
                    std::string rarityText;
                    switch (item->getRarity()) {
                        case ItemRarity::COMMON:
                            rarityText = "Common";
                            break;
                        case ItemRarity::UNCOMMON:
                            rarityText = "Uncommon";
                            break;
                        case ItemRarity::RARE:
                            rarityText = "Rare";
                            break;
                        case ItemRarity::EPIC:
                            rarityText = "Epic";
                            break;
                        case ItemRarity::LEGENDARY:
                            rarityText = "Legendary";
                            break;
                    }
                    
                    std::cout << "Picked up: [" << rarityText << "] " << item->getName() << " - " << item->getDescription() << std::endl;
                }
            }
        }
    } else {
        pickupKeyPressed = false;
    }
    
    // Use health potion
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        // Find a health potion in inventory
        for (const auto& item : player->getInventory()) {
            auto potion = std::dynamic_pointer_cast<Potion>(item);
            if (potion) {
                potion->use();
                player->heal(potion->getHealAmount());
                player->removeItem(item);
                std::cout << "Used health potion. Health: " << player->getHealth() << "/" << player->getMaxHealth() << std::endl;
                break;
            }
        }
    }
}

void GameLoop::update(float deltaTime) {
    // Update based on current game state
    switch (currentState) {
        case GameState::CHARACTER_SELECT:
            updateCharacterSelect(deltaTime);
            break;
            
        case GameState::PLAYING:
            // In PLAYING state, we should always have player and level initialized
            // But check just to be safe
            if (!player || !currentLevel) {
                std::cerr << "Error: Player or level not initialized in PLAYING state" << std::endl;
                currentState = GameState::CHARACTER_SELECT;
                characterSelectScreen = std::make_unique<CharacterSelectScreen>(uiSystem.get());
                break;
            }
            
            // Update player position based on input
            handlePlayerMovement(deltaTime);
            
            // Handle player actions (attacking, etc.)
            handlePlayerActions();
            
            // Update level (includes enemy AI, etc.)
            currentLevel->update(deltaTime, player.get());
            
            // Update visual effects
            effectManager.update(deltaTime);
            
            // Update inventory UI
            inventoryUI->update(deltaTime);
            
            // Update camera position to follow player
            if (renderer) {
                renderer->setCameraPosition(player->getX(), player->getY());
            }
            
            // Debug output
            std::cout << "Player position: " << player->getX() << ", " << player->getY() 
                      << " | Health: " << player->getHealth() << "/" << player->getMaxHealth()
                      << " | Enemies: " << currentLevel->getEnemies().size() << std::endl;
            
            // Check for game over condition
            if (player->getHealth() <= 0) {
                currentState = GameState::GAME_OVER;
                std::cout << "Game Over!" << std::endl;
            }
            break;
            
        case GameState::GAME_OVER:
        case GameState::PAUSE_MENU:
            // TODO: Implement these states
            break;
    }
}

void GameLoop::render() {
    // Render based on current game state
    switch (currentState) {
        case GameState::CHARACTER_SELECT:
            renderCharacterSelect();
            break;
            
        case GameState::PLAYING:
            // Make sure level and player are initialized before rendering
            if (currentLevel && player && renderer) {
                // Combine all vertices for a single render call
                std::vector<Vertex> allVertices;
                
                // Get game world vertices
                std::vector<Vertex> gameWorldVertices = renderer->generateGameWorldVertices(currentLevel, player, effectManager);
                allVertices.insert(allVertices.end(), gameWorldVertices.begin(), gameWorldVertices.end());
                
                // Add inventory UI vertices if visible
                if (inventoryUI->isInventoryVisible()) {
                    std::vector<Vertex> inventoryVertices = inventoryUI->generateInventoryVertices(player);
                    if (!inventoryVertices.empty()) {
                        std::cout << "Adding inventory UI with " << inventoryVertices.size() << " vertices" << std::endl;
                        allVertices.insert(allVertices.end(), inventoryVertices.begin(), inventoryVertices.end());
                    }
                }
                
                // Update vertex buffer and render everything in a single call
                if (!allVertices.empty()) {
                    renderer->updateVertexBuffer(allVertices);
                    vulkanRenderer->render();
                }
            }
            break;
            
        case GameState::GAME_OVER:
        case GameState::PAUSE_MENU:
            // TODO: Implement these states
            break;
    }
}

void GameLoop::updateCharacterSelect(float deltaTime) {
    // Update the character selection screen
    characterSelectScreen->update(deltaTime);
    
    // Check if a character has been selected and confirmed
    if (characterSelectScreen->isSelectionConfirmed()) {
        // Create the player character based on the selected class
        createPlayerCharacter(characterSelectScreen->getSelectedClass());
        
        // Start the game
        startGame();
    }
}

void GameLoop::renderCharacterSelect() {
    // Generate vertices for the character selection screen
    std::vector<Vertex> vertices = characterSelectScreen->generateVertices(uiSystem.get());
    
    // Debug output
    std::cout << "Rendering character selection screen with " << vertices.size() << " vertices" << std::endl;
    
    if (renderer) {
        renderer->updateVertexBuffer(vertices); // This will call createVertexBuffer, which in turn calls vulkanRenderer->setCurrentVertexBuffer
    }
    vulkanRenderer->render();
}

bool GameLoop::isKeyJustPressed(int key) {
    bool currentlyPressed = (glfwGetKey(window, key) == GLFW_PRESS);
    bool wasPressedPreviously = previousKeyStates.count(key) ? previousKeyStates[key] : false;

    // Update the state for this key for the next frame's check
    previousKeyStates[key] = currentlyPressed;

    return currentlyPressed && !wasPressedPreviously;
}

void GameLoop::handleCharacterSelectInput() {
    // Check for key press events
    if (isKeyJustPressed(GLFW_KEY_LEFT) || isKeyJustPressed(GLFW_KEY_A)) {
        characterSelectScreen->handleInput(GLFW_KEY_LEFT);
    } else if (isKeyJustPressed(GLFW_KEY_RIGHT) || isKeyJustPressed(GLFW_KEY_D)) {
        characterSelectScreen->handleInput(GLFW_KEY_RIGHT);
    } else if (isKeyJustPressed(GLFW_KEY_ENTER) || isKeyJustPressed(GLFW_KEY_SPACE)) {
        characterSelectScreen->handleInput(GLFW_KEY_ENTER);
    }
}

void GameLoop::createPlayerCharacter(CharacterClass characterClass) {
    // Create a new player character based on the selected class
    std::string className;
    switch (characterClass) {
        case CharacterClass::WARRIOR:
            className = "Warrior";
            break;
        case CharacterClass::RANGER:
            className = "Ranger";
            break;
        case CharacterClass::MAGE:
            className = "Mage";
            break;
    }
    
    player = std::make_shared<Character>("Player", characterClass);
    std::cout << "Created " << className << " character!" << std::endl;
}

void GameLoop::startGame() {
    // Initialize the game world
    currentLevel = std::make_shared<Level>(100, 100);
    currentLevel->generateDungeon();
    
    // Place the player in a valid starting position
    for (int y = 0; y < currentLevel->getHeight(); y++) {
        for (int x = 0; x < currentLevel->getWidth(); x++) {
            if (currentLevel->isWalkable(x, y)) {
                player->move(static_cast<float>(x), static_cast<float>(y));
                std::cout << "Placed player at starting position: (" << x << ", " << y << ")" << std::endl;
                break;
            }
        }
    }
    
    // Give player some starting items based on their class
    std::cout << "Adding starting items to player..." << std::endl;
    try {
        // Common items for all classes
        player->addItem(std::make_shared<Potion>("Small Health Potion", ItemRarity::COMMON, 10));
        
        // Class-specific starting items
        switch (player->getClass()) {
            case CharacterClass::WARRIOR:
                player->addItem(std::make_shared<Weapon>("Iron Sword", ItemRarity::COMMON, 6));
                player->addItem(std::make_shared<Armor>("Leather Armor", ItemRarity::COMMON, 4));
                break;
                
            case CharacterClass::RANGER:
                player->addItem(std::make_shared<Weapon>("Wooden Bow", ItemRarity::COMMON, 5));
                player->addItem(std::make_shared<Armor>("Light Leather Armor", ItemRarity::COMMON, 3));
                break;
                
            case CharacterClass::MAGE:
                player->addItem(std::make_shared<Weapon>("Apprentice Staff", ItemRarity::COMMON, 7));
                player->addItem(std::make_shared<Armor>("Cloth Robe", ItemRarity::COMMON, 2));
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error adding items to player: " << e.what() << std::endl;
        // Continue even if items fail to add
    }
    
    // Add some enemies to the level
    for (int i = 0; i < 10; i++) {
        // Find a valid position for the enemy
        int x, y;
        do {
            x = rand() % currentLevel->getWidth();
            y = rand() % currentLevel->getHeight();
        } while (!currentLevel->isWalkable(x, y) || 
                (std::abs(x - player->getX()) < 5 && std::abs(y - player->getY()) < 5));
        
        // Create and add the enemy
        auto enemy = std::make_shared<Enemy>("Enemy", EnemyType::GOBLIN, 1);
        enemy->setLevel(currentLevel.get());
        currentLevel->addEnemy(enemy);
    }
    
    // Change the game state to playing
    currentState = GameState::PLAYING;
    std::cout << "Game started!" << std::endl;
}

void GameLoop::run() {
    try {
        std::cout << "Starting GameLoop::run..." << std::endl;
        
        if (!initialize()) {
            std::cerr << "Failed to initialize game loop" << std::endl;
            return;
        }
        
        // Set the initial game state to character selection
        currentState = GameState::CHARACTER_SELECT;
        characterSelectScreen = std::make_unique<CharacterSelectScreen>(uiSystem.get());
        std::cout << "Character selection screen initialized" << std::endl;
        
        std::cout << "Starting game loop" << std::endl;
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        // Skip initial animation frames and go straight to the main game loop
        std::cout << "Starting main game loop..." << std::endl;
        
        // Display a message in the console
        std::cout << "\nWindow is now open. Close the window or press ESC to exit." << std::endl;
        
        // FPS counter variables
        int frameCount = 0;
        float fpsTimer = 0.0f;
        float fps = 0.0f;
        
        // Target frame rate (can be adjusted)
        const int TARGET_FPS = 120; // Increased from 60 for smoother gameplay
        const float FRAME_TIME = 1.0f / TARGET_FPS;
        
        // Continue the game loop until the user closes the window or presses ESC
        while (!glfwWindowShouldClose(window)) {
            // Start frame timing
            auto frameStart = std::chrono::high_resolution_clock::now();
            
            // Calculate delta time
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
            
            // Cap delta time to prevent large jumps if game freezes momentarily
            if (deltaTime > 0.1f) {
                deltaTime = 0.1f;
            }
            
            lastTime = currentTime;
            
            // Update FPS counter
            frameCount++;
            fpsTimer += deltaTime;
            if (fpsTimer >= 1.0f) {
                fps = frameCount / fpsTimer;
                frameCount = 0;
                fpsTimer = 0.0f;
                
                // Update window title with FPS
                std::string title = std::string(WINDOW_TITLE) + " - FPS: " + std::to_string(static_cast<int>(fps));
                glfwSetWindowTitle(window, title.c_str());
            }
            
            // Poll for events
            glfwPollEvents();
            
            // Process input
            processInput();
            
            // Update game state - ensure this runs regardless of input
            update(deltaTime);
            
            // Render the game
            render();
            
            // Calculate how long the frame took to process
            auto frameEnd = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(frameEnd - frameStart).count();
            
            // Sleep only if needed to maintain target frame rate
            if (frameTime < FRAME_TIME) {
                std::this_thread::sleep_for(std::chrono::duration<float, std::chrono::seconds::period>(FRAME_TIME - frameTime));
            }
        }
        
        std::cout << "Window closed, waiting for device to finish operations..." << std::endl;
        // Wait for the device to finish operations before cleanup
        vkDeviceWaitIdle(vulkanRenderer->getDevice());
        std::cout << "Device operations finished" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in GameLoop::run: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in GameLoop::run" << std::endl;
    }
} 