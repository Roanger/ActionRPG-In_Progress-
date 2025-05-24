#pragma once

#include <string>

enum class ItemType {
    WEAPON,
    ARMOR,
    POTION,
    SCROLL,
    MISC
};

enum class ItemRarity {
    COMMON,
    UNCOMMON,
    RARE,
    EPIC,
    LEGENDARY
};

class Item {
public:
    Item(const std::string& name, ItemType type, ItemRarity rarity);
    virtual ~Item();
    
    // Getters
    const std::string& getName() const { return name; }
    ItemType getType() const { return type; }
    ItemRarity getRarity() const { return rarity; }
    int getValue() const { return value; }
    float getX() const { return x; }
    float getY() const { return y; }
    
    // Setters
    void setPosition(float newX, float newY) { x = newX; y = newY; }
    
    // Virtual methods
    virtual void use() = 0;
    virtual std::string getDescription() const = 0;
    
protected:
    std::string name;
    ItemType type;
    ItemRarity rarity;
    int value;
    float x, y; // Position in the game world
};

class Weapon : public Item {
public:
    Weapon(const std::string& name, ItemRarity rarity, int damage);
    ~Weapon() override;
    
    int getDamage() const { return damage; }
    
    void use() override;
    std::string getDescription() const override;
    
private:
    int damage;
};

class Armor : public Item {
public:
    Armor(const std::string& name, ItemRarity rarity, int defense);
    ~Armor() override;
    
    int getDefense() const { return defense; }
    
    void use() override;
    std::string getDescription() const override;
    
private:
    int defense;
};

class Potion : public Item {
public:
    Potion(const std::string& name, ItemRarity rarity, int healAmount);
    ~Potion() override;
    
    int getHealAmount() const { return healAmount; }
    
    void use() override;
    std::string getDescription() const override;
    
private:
    int healAmount;
}; 