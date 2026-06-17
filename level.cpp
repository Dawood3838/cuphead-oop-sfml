#include "level.h"
#include <cmath>

Platform::Platform() : active(false), isMoving(false), moveSpeed(0), moveRange(0), platformTexture(nullptr), useTexture(false) {}

void Platform::initialize(float x, float y, float width, float height, Color color)
{
	shape.setSize(Vector2f(width, height));
	shape.setPosition(x, y);
	shape.setFillColor(color);
	shape.setOutlineThickness(2);
	shape.setOutlineColor(Color::Black);
	active = true;
	isMoving = false;
	useTexture = false;
}

void Platform::initializeWithTexture(float x, float y, float width, float height, Texture* texture)
{
	shape.setSize(Vector2f(width, height));
	shape.setPosition(x, y);
	shape.setFillColor(Color::White);
	shape.setOutlineThickness(2);
	shape.setOutlineColor(Color::Black);
	active = true;
	isMoving = false;
	platformTexture = texture;
	useTexture = true;
	
	// Setup sprite with texture
	if (platformTexture)
	{
		spriteVisual.setTexture(*platformTexture);
		spriteVisual.setPosition(x, y);
		spriteVisual.setScale(width / platformTexture->getSize().x, 
		                       height / platformTexture->getSize().y);
		// Use brown color multiplier to ignore white background
		// This makes white (255,255,255) blend with brown color instead
		spriteVisual.setColor(Color(200, 150, 100, 255));  // Brown tint to filter white
	}
}

void Platform::initializeMoving(float x, float y, float width, float height, float speed, float range, Color color)
{
	initialize(x, y, width, height, color);
	isMoving = true;
	moveSpeed = speed;
	moveRange = range;
}

void Platform::update()
{
	if (isMoving)
	{
		float posY = shape.getPosition().y;
		posY += moveSpeed;
		// Bounce up and down within range
		if (posY < shape.getPosition().y - moveRange || posY > shape.getPosition().y + moveRange)
			moveSpeed *= -1;
		shape.setPosition(shape.getPosition().x, posY);
		
		if (useTexture && platformTexture)
		{
			spriteVisual.setPosition(shape.getPosition());
		}
	}
}

void Platform::draw(RenderWindow& window)
{
	if (active)
	{
		if (useTexture && platformTexture)
		{
			window.draw(spriteVisual);
		}
		else
		{
			window.draw(shape);
		}
	}
}

FloatRect Platform::getBounds() const { return shape.getGlobalBounds(); }
bool Platform::isColliding(FloatRect rect) const { return active && shape.getGlobalBounds().intersects(rect); }
float Platform::getTopY() const { return shape.getPosition().y; }
Vector2f Platform::getPosition() const { return shape.getPosition(); }
bool Platform::getActive() const { return active; }

// ==================== GOAL FLAG IMPLEMENTATION ====================
GoalFlag::GoalFlag() : reached(false), rotation(0) {}

void GoalFlag::initialize(float x, float y)
{
	// Pole
	flagPole.setRadius(5);
	flagPole.setPosition(x, y);
	flagPole.setFillColor(Color::Black);

	// Banner (red/gold flag)
	flagBanner.setSize(Vector2f(40, 30));
	flagBanner.setPosition(x + 10, y - 25);
	flagBanner.setFillColor(Color(255, 215, 0));  // Gold
	flagBanner.setOutlineThickness(2);
	flagBanner.setOutlineColor(Color::Black);

	reached = false;
	rotation = 0;
}

void GoalFlag::update()
{
	rotation += 2;
	if (rotation > 360) rotation = 0;
}

void GoalFlag::draw(RenderWindow& window)
{
	window.draw(flagPole);

	// Draw flag with slight animation
	flagBanner.setFillColor(Color(255, 200 + (int)(50 * sin(rotation * 3.14f / 180)), 0));
	window.draw(flagBanner);
}

FloatRect GoalFlag::getBounds() const
{
	FloatRect poleBounds = flagPole.getGlobalBounds();
	FloatRect flagBounds = flagBanner.getGlobalBounds();
	return FloatRect(poleBounds.left, poleBounds.top, poleBounds.width + flagBounds.width, 100);
}

bool GoalFlag::checkCollision(FloatRect playerBounds) const { return getBounds().intersects(playerBounds); }
bool GoalFlag::isReached() const { return reached; }
void GoalFlag::setReached(bool r) { reached = r; }

// ==================== SPAWN POINT IMPLEMENTATION ====================
SpawnPoint::SpawnPoint() : enemyType(DEADLY_DAISY), used(false) {}

void SpawnPoint::initialize(float x, float y, EnemyType type)
{
	marker.setRadius(8);
	marker.setPosition(x, y);
	marker.setFillColor(Color(200, 100, 100, 50));  // Semi-transparent red
	marker.setOutlineThickness(1);
	marker.setOutlineColor(Color::Red);
	enemyType = type;
	used = false;
}

EnemyType SpawnPoint::getEnemyType() const { return enemyType; }
Vector2f SpawnPoint::getPosition() const { return marker.getPosition(); }
bool SpawnPoint::isUsed() const { return used; }
void SpawnPoint::setUsed(bool u) { used = u; }
