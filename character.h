#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

// ==================== CHARACTER BASE CLASS ====================
class Character
{
protected:
	int maxHealth;
	int currentHealth;
	float speed;
	float jumpPower;
	string name;
	Color characterColor;

public:
	Character(const string& n, int health, float spd, float jump, Color color);
	virtual ~Character();

	int getMaxHealth() const;
	int getHealth() const;
	float getSpeed() const;
	float getJumpPower() const;
	string getName() const;
	Color getColor() const;

	void takeDamage(int damage);
	void heal(int amount);
	void resetHealth();
};

// ==================== CUPHEAD CLASS ====================
class Cuphead : public Character
{
public:
	Cuphead();
};

// ==================== MUGMAN CLASS ====================
class Mugman : public Character
{
public:
	Mugman();
};

#endif // CHARACTER_H
