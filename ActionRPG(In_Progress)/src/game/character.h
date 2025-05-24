#pragma once

#include <string>
#include <vector>
#include <memory>

class Item;
class Weapon; // Forward declaration for Weapon
class Armor;  // Forward declaration for Armor

enum class CharacterClass {
    WARRIOR,
    RANGER,
    MAGE
};

class Character {
public:
    Character(const std::string& name, CharacterClass characterClass);
    ~Character();
    
    // Getters
    const std::string& getName() const { return name; }
    CharacterClass getClass() const { return characterClass; }
    int getLevel() const { return level; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getMana() const { return mana; }
    int getMaxMana() const { return maxMana; }
    int getStrength() const { return strength; }
    int getDexterity() const { return dexterity; }
    int getIntelligence() const { return intelligence; }
    float getX() const { return x; }
    float getY() const { return y; }
    
    // Attack types
    enum class AttackType {
        MELEE,
        RANGED
    };
    
    // Visual effect types
    enum class VisualEffectType {
        NONE,
        SLASH,
        ARROW,
        FIREBALL
    };
    
    // Actions
    void move(float dx, float dy);
    void attack(Character* target);
    void rangedAttack(Character* target, float targetX, float targetY);
    AttackType getDefaultAttackType() const;
    VisualEffectType getAttackVisualEffect() const;
    void takeDamage(int damage);
    void heal(int amount);
    void levelUp();
    bool isDead() const { return health <= 0; }
    
    // Attack properties
    float getAttackRange() const;
    int getAttackDamage() const;
    
    // Inventory
    void addItem(std::shared_ptr<Item> item);
    void removeItem(std::shared_ptr<Item> item);
    void removeItem(int index);
    const std::vector<std::shared_ptr<Item>>& getInventory() const { return inventory; }
    
    // Equip functions
    void equipWeapon(std::shared_ptr<Weapon> weapon);
    void equipArmor(std::shared_ptr<Armor> armor);
    
protected:
    std::string name;
    CharacterClass characterClass;
    int level;
    int experience;
    int health;
    int maxHealth;
    int mana;
    int maxMana;
    int strength;
    int dexterity;
    int intelligence;
    float x, y; // Position
    
    std::vector<std::shared_ptr<Item>> inventory;
    
    void initializeStats();
}; 