#include "character.h"
#include "item.h"
#include <iostream>
#include <algorithm>

Character::Character(const std::string& name, CharacterClass characterClass)
    : name(name), characterClass(characterClass), level(1), experience(0), x(0.0f), y(0.0f) {
    initializeStats();
}

Character::~Character() {
}

void Character::initializeStats() {
    // Base stats depending on character class
    switch (characterClass) {
        case CharacterClass::WARRIOR:
            strength = 10;
            dexterity = 5;
            intelligence = 3;
            maxHealth = 100;
            maxMana = 30;
            break;
        case CharacterClass::RANGER:
            strength = 5;
            dexterity = 10;
            intelligence = 5;
            maxHealth = 80;
            maxMana = 50;
            break;
        case CharacterClass::MAGE:
            strength = 3;
            dexterity = 5;
            intelligence = 10;
            maxHealth = 60;
            maxMana = 100;
            break;
    }
    
    // Initialize current values
    health = maxHealth;
    mana = maxMana;
}

void Character::move(float newX, float newY) {
    // Use absolute positioning instead of relative
    x = newX;
    y = newY;
    
    // Debug output to track character movement
    std::cout << "Character " << name << " moved to (" << x << ", " << y << ")" << std::endl;
}

Character::AttackType Character::getDefaultAttackType() const {
    switch (characterClass) {
        case CharacterClass::WARRIOR:
            return AttackType::MELEE;
        case CharacterClass::RANGER:
        case CharacterClass::MAGE:
            return AttackType::RANGED;
        default:
            return AttackType::MELEE;
    }
}

Character::VisualEffectType Character::getAttackVisualEffect() const {
    switch (characterClass) {
        case CharacterClass::WARRIOR:
            return VisualEffectType::SLASH;
        case CharacterClass::RANGER:
            return VisualEffectType::ARROW;
        case CharacterClass::MAGE:
            return VisualEffectType::FIREBALL;
        default:
            return VisualEffectType::NONE;
    }
}

float Character::getAttackRange() const {
    switch (characterClass) {
        case CharacterClass::WARRIOR:
            return 1.5f; // Melee range
        case CharacterClass::RANGER:
            return 5.0f; // Medium range
        case CharacterClass::MAGE:
            return 7.0f; // Long range
        default:
            return 1.5f;
    }
}

int Character::getAttackDamage() const {
    switch (characterClass) {
        case CharacterClass::WARRIOR:
            return strength * 2;
        case CharacterClass::RANGER:
            return dexterity * 2;
        case CharacterClass::MAGE:
            return intelligence * 2;
        default:
            return 1;
    }
}

void Character::attack(Character* target) {
    if (!target || target->isDead()) return;
    
    // Check if target is in range for melee attack
    float dx = target->getX() - x;
    float dy = target->getY() - y;
    float distanceSquared = dx * dx + dy * dy;
    
    // If this is a melee character and target is out of range, don't attack
    if (getDefaultAttackType() == AttackType::MELEE && distanceSquared > getAttackRange() * getAttackRange()) {
        std::cout << name << " is too far to attack " << target->getName() << "!" << std::endl;
        return;
    }
    
    int damage = getAttackDamage();
    
    // Apply damage to target
    target->takeDamage(damage);
    
    std::cout << name << " attacks " << target->getName() << " with a " 
              << (getDefaultAttackType() == AttackType::MELEE ? "melee" : "ranged") 
              << " attack for " << damage << " damage!" << std::endl;
    
    // Check if target died
    if (target->isDead()) {
        std::cout << target->getName() << " has been defeated!" << std::endl;
        // Award experience (to be implemented)
    }
}

void Character::rangedAttack(Character* target, float targetX, float targetY) {
    if (!target || target->isDead()) return;
    
    // Only ranged classes can use ranged attacks
    if (getDefaultAttackType() != AttackType::RANGED) {
        std::cout << name << " cannot perform ranged attacks!" << std::endl;
        return;
    }
    
    // Check if target is in range
    float dx = targetX - x;
    float dy = targetY - y;
    float distanceSquared = dx * dx + dy * dy;
    
    if (distanceSquared > getAttackRange() * getAttackRange()) {
        std::cout << name << " cannot reach the target with a ranged attack!" << std::endl;
        return;
    }
    
    int damage = getAttackDamage();
    
    // Apply damage to target
    target->takeDamage(damage);
    
    // Describe the attack based on character class
    std::string attackDescription;
    switch (characterClass) {
        case CharacterClass::RANGER:
            attackDescription = "fires an arrow at";
            break;
        case CharacterClass::MAGE:
            attackDescription = "casts a fireball at";
            break;
        default:
            attackDescription = "attacks";
    }
    
    std::cout << name << " " << attackDescription << " " << target->getName() << " for " << damage << " damage!" << std::endl;
    
    // Check if target died
    if (target->isDead()) {
        std::cout << target->getName() << " has been defeated!" << std::endl;
        // Award experience (to be implemented)
    }
}

void Character::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

void Character::heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void Character::levelUp() {
    level++;
    
    // Increase stats based on character class
    switch (characterClass) {
        case CharacterClass::WARRIOR:
            strength += 3;
            dexterity += 1;
            intelligence += 1;
            maxHealth += 20;
            maxMana += 5;
            break;
        case CharacterClass::RANGER:
            strength += 1;
            dexterity += 3;
            intelligence += 1;
            maxHealth += 15;
            maxMana += 10;
            break;
        case CharacterClass::MAGE:
            strength += 1;
            dexterity += 1;
            intelligence += 3;
            maxHealth += 10;
            maxMana += 20;
            break;
    }
    
    // Restore health and mana on level up
    health = maxHealth;
    mana = maxMana;
    
    std::cout << name << " has reached level " << level << "!" << std::endl;
}

void Character::addItem(std::shared_ptr<Item> item) {
    inventory.push_back(item);
}

void Character::removeItem(std::shared_ptr<Item> item) {
    // Find and remove the item from the inventory
    auto it = std::remove(inventory.begin(), inventory.end(), item);
    if (it != inventory.end()) {
        inventory.erase(it, inventory.end());
    }
}

void Character::removeItem(int index) {
    if (index >= 0 && index < inventory.size()) {
        inventory.erase(inventory.begin() + index);
    }
}

void Character::equipWeapon(std::shared_ptr<Weapon> weapon) {
    // For now, just print a message. Actual equipping logic (e.g., updating stats, unequipping old weapon) can be added here.
    std::cout << name << " equipped weapon: " << weapon->getName() << std::endl;
    // Example: this->equippedWeapon = weapon;
}

void Character::equipArmor(std::shared_ptr<Armor> armor) {
    // For now, just print a message. Actual equipping logic can be added here.
    std::cout << name << " equipped armor: " << armor->getName() << std::endl;
    // Example: this->equippedArmor = armor;
}