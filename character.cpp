#include "character.h"

Character::Character(const string& n, int health, float spd, float jump, Color color)
	: maxHealth(health), currentHealth(health), speed(spd), jumpPower(jump), name(n), characterColor(color) {}

Character::~Character() {}

int Character::getMaxHealth() const { return maxHealth; }
int Character::getHealth() const { return currentHealth; }
float Character::getSpeed() const { return speed; }
float Character::getJumpPower() const { return jumpPower; }
string Character::getName() const { return name; }
Color Character::getColor() const { return characterColor; }

void Character::takeDamage(int damage)
{
	currentHealth -= damage;
	if (currentHealth < 0) currentHealth = 0;
}

void Character::heal(int amount)
{
	currentHealth += amount;
	if (currentHealth > maxHealth) currentHealth = maxHealth;
}

void Character::resetHealth() { currentHealth = maxHealth; }

Cuphead::Cuphead() : Character("Cuphead", 100, 6.0f, 22.0f, Color(173, 216, 230)) {}

Mugman::Mugman() : Character("Mugman", 120, 5.5f, 20.0f, Color(173, 216, 230)) {}
