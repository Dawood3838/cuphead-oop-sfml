#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include "constants.h"

using namespace sf;

// ==================== PLATFORM CLASS ====================
class Platform
{
private:
	RectangleShape shape;
	bool active;
	bool isMoving;
	float moveSpeed;
	float moveRange;
	Texture* platformTexture;
	Sprite spriteVisual;
	bool useTexture;

public:
	Platform();

	void initialize(float x, float y, float width, float height, Color color = Color::Green);
	void initializeWithTexture(float x, float y, float width, float height, Texture* texture);
	void initializeMoving(float x, float y, float width, float height, float speed, float range, Color color = Color::Green);
	void update();
	void draw(RenderWindow& window);

	FloatRect getBounds() const;
	bool isColliding(FloatRect rect) const;
	float getTopY() const;
	Vector2f getPosition() const;
	bool getActive() const;
};

// ==================== GOAL FLAG CLASS ====================
class GoalFlag
{
private:
	CircleShape flagPole;
	RectangleShape flagBanner;
	bool reached;
	float rotation;

public:
	GoalFlag();

	void initialize(float x, float y);
	void update();
	void draw(RenderWindow& window);

	FloatRect getBounds() const;
	bool checkCollision(FloatRect playerBounds) const;
	bool isReached() const;
	void setReached(bool r);
};

// ==================== SPAWN POINT CLASS ====================
class SpawnPoint
{
private:
	CircleShape marker;
	EnemyType enemyType;
	bool used;

public:
	SpawnPoint();

	void initialize(float x, float y, EnemyType type);

	EnemyType getEnemyType() const;
	Vector2f getPosition() const;
	bool isUsed() const;
	void setUsed(bool u);
};

#endif // LEVEL_H
