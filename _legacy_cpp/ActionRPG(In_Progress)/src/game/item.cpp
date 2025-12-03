#include "item.h"
#include <iostream>
#include <sstream>

// Base Item implementation
Item::Item(const std::string& name, ItemType type, ItemRarity rarity)
    : name(name), type(type), rarity(rarity) {
    
    // Calculate base value based on rarity
    switch (rarity) {
        case ItemRarity::COMMON:
            value = 10;
            break;
        case ItemRarity::UNCOMMON:
            value = 25;
            break;
        case ItemRarity::RARE:
            value = 100;
            break;
        case ItemRarity::EPIC:
            value = 500;
            break;
        case ItemRarity::LEGENDARY:
            value = 2000;
            break;
    }
}

Item::~Item() {
}

// Weapon implementation
Weapon::Weapon(const std::string& name, ItemRarity rarity, int damage)
    : Item(name, ItemType::WEAPON, rarity), damage(damage) {
    // Adjust value based on damage
    value += damage * 5;
}

Weapon::~Weapon() {
}

void Weapon::use() {
    std::cout << "You brandish the " << name << "." << std::endl;
}

std::string Weapon::getDescription() const {
    std::stringstream ss;
    
    // Add rarity color (in a real game, we'd use actual colors)
    switch (rarity) {
        case ItemRarity::COMMON:
            ss << "[Common] ";
            break;
        case ItemRarity::UNCOMMON:
            ss << "[Uncommon] ";
            break;
        case ItemRarity::RARE:
            ss << "[Rare] ";
            break;
        case ItemRarity::EPIC:
            ss << "[Epic] ";
            break;
        case ItemRarity::LEGENDARY:
            ss << "[Legendary] ";
            break;
    }
    
    ss << name << "\n";
    ss << "Damage: " << damage << "\n";
    ss << "Value: " << value << " gold";
    
    return ss.str();
}

// Armor implementation
Armor::Armor(const std::string& name, ItemRarity rarity, int defense)
    : Item(name, ItemType::ARMOR, rarity), defense(defense) {
    // Adjust value based on defense
    value += defense * 7;
}

Armor::~Armor() {
}

void Armor::use() {
    std::cout << "You equip the " << name << "." << std::endl;
}

std::string Armor::getDescription() const {
    std::stringstream ss;
    
    // Add rarity color
    switch (rarity) {
        case ItemRarity::COMMON:
            ss << "[Common] ";
            break;
        case ItemRarity::UNCOMMON:
            ss << "[Uncommon] ";
            break;
        case ItemRarity::RARE:
            ss << "[Rare] ";
            break;
        case ItemRarity::EPIC:
            ss << "[Epic] ";
            break;
        case ItemRarity::LEGENDARY:
            ss << "[Legendary] ";
            break;
    }
    
    ss << name << "\n";
    ss << "Defense: " << defense << "\n";
    ss << "Value: " << value << " gold";
    
    return ss.str();
}

// Potion implementation
Potion::Potion(const std::string& name, ItemRarity rarity, int healAmount)
    : Item(name, ItemType::POTION, rarity), healAmount(healAmount) {
    // Adjust value based on heal amount
    value += healAmount * 2;
}

Potion::~Potion() {
}

void Potion::use() {
    std::cout << "You drink the " << name << " and restore " << healAmount << " health." << std::endl;
}

std::string Potion::getDescription() const {
    std::stringstream ss;
    
    // Add rarity color
    switch (rarity) {
        case ItemRarity::COMMON:
            ss << "[Common] ";
            break;
        case ItemRarity::UNCOMMON:
            ss << "[Uncommon] ";
            break;
        case ItemRarity::RARE:
            ss << "[Rare] ";
            break;
        case ItemRarity::EPIC:
            ss << "[Epic] ";
            break;
        case ItemRarity::LEGENDARY:
            ss << "[Legendary] ";
            break;
    }
    
    ss << name << "\n";
    ss << "Heals: " << healAmount << " HP\n";
    ss << "Value: " << value << " gold";
    
    return ss.str();
} 