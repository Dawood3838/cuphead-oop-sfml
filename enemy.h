#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include "constants.h"

using namespace sf;

// ==================== ENEMY CLASS ====================
class Enemy
{
private:
	Sprite sprite;
	Texture texture;
	float velocityX;
	float velocityY;
	bool active;
	int health;
	int maxHealth;
	float moveTimer;
	float moveDirection;
	EnemyType enemyType;
	float attackTimer;
	float attackCooldown;
	Color enemyColor;
	float size;
	float speedMultiplier;

public:
	Enemy();

	void initialize(float x, float y, EnemyType type, float speedMultiplier = 1.0f);
	void update();
	void takeDamage(int damage);
	void draw(RenderWindow& window);

	bool isActive() const;
	FloatRect getBounds() const;
	Vector2f getPosition() const;
	int getHealth() const;
	int getMaxHealth() const;
	EnemyType getType() const;
};

#endif // ENEMY_H
