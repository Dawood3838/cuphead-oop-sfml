#include "enemy.h"
#include <cmath>
#include <iostream>

Enemy::Enemy() : velocityX(0), velocityY(0), active(false), health(30), maxHealth(30), 
                 moveTimer(0), moveDirection(1.0f), enemyType(DEADLY_DAISY), 
                 attackTimer(0), attackCooldown(1.0f), speedMultiplier(1.0f) {}

void Enemy::initialize(float x, float y, EnemyType type, float speedMultiplier)
{
	sprite.setPosition(x, y);
	enemyType = type;
	moveTimer = 0;
	moveDirection = -1.0f;
	velocityY = 0;
	attackTimer = 0;
	this->speedMultiplier = speedMultiplier;

	// Load texture based on enemy type
	std::string texturePath = "";
	switch (type)
	{
		case DEADLY_DAISY:
			maxHealth = 20;
			size = 8.0f;
			enemyColor = Color::Yellow;
			attackCooldown = 2.0f;
			texturePath = "Data/flowergrunt.png";
			break;
		case AGGRAVATING_ACORN:
			maxHealth = 35;
			size = 10.0f;
			enemyColor = Color(139, 69, 19);
			attackCooldown = 1.5f;
			texturePath = "Data/acorn.png";
			break;
		case MURDEROUS_MUSHROOM:
			maxHealth = 50;
			size = 12.0f;
			enemyColor = Color(255, 20, 147);
			attackCooldown = 2.5f;
			texturePath = "Data/mushroom.png";
			break;
		case TERRIBLE_TULIP:
			maxHealth = 30;
			size = 9.0f;
			enemyColor = Color(255, 105, 180);
			attackCooldown = 1.2f;
			texturePath = "Data/flowergrunt.png";
			break;
		case TOOTHY_TERROR:
			maxHealth = 60;
			size = 14.0f;
			enemyColor = Color::Red;
			attackCooldown = 1.0f;
			texturePath = "Data/chomper.png";
			break;
		case BOTHERSOME_BLUEBERRY:
			maxHealth = 25;
			size = 8.0f;
			enemyColor = Color::Blue;
			attackCooldown = 0.8f;
			texturePath = "Data/blob.png";
			break;
		case ACORN_MAKER:
			maxHealth = 120;
			size = 18.0f;
			enemyColor = Color(205, 133, 63);
			attackCooldown = 0.5f;
			texturePath = "Data/machine.png";
			break;
	}

	// Load texture
	if (texture.loadFromFile(texturePath))
	{
		sprite.setTexture(texture);
		// Scale the sprite to fit the enemy size
		sprite.setScale(0.35f, 0.35f);  // Increased from 0.15f for larger enemies
	}
	else
	{
		std::cout << "Warning: Could not load enemy texture: " << texturePath << std::endl;
	}

	health = maxHealth;
	active = true;
}

void Enemy::update()
{
	if (!active) return;

	attackTimer += 0.016f;

	switch (enemyType)
	{
		case DEADLY_DAISY:
			moveTimer++;
			if (moveTimer > 180)
			{
				moveDirection *= -1.0f;
				moveTimer = 0;
			}
			velocityX = moveDirection * 1.5f * speedMultiplier;
			break;

		case AGGRAVATING_ACORN:
			moveTimer++;
			if (moveTimer > 200)
			{
				moveDirection *= -1.0f;
				moveTimer = 0;
			}
			velocityX = moveDirection * 2.0f * speedMultiplier;
			break;

		case MURDEROUS_MUSHROOM:
			moveTimer++;
			if (moveTimer > 240)
			{
				moveDirection *= -1.0f;
				moveTimer = 0;
			}
			velocityX = moveDirection * 1.2f * speedMultiplier;
			break;

		case TERRIBLE_TULIP:
			moveTimer++;
			if (moveTimer > 120)
			{
				moveDirection *= -1.0f;
				moveTimer = 0;
			}
			velocityX = moveDirection * 3.5f * speedMultiplier;
			break;

		case TOOTHY_TERROR:
			moveTimer++;
			if (moveTimer > 160)
			{
				moveDirection *= -1.0f;
				moveTimer = 0;
			}
			velocityX = moveDirection * 2.5f * speedMultiplier;
			break;

		case BOTHERSOME_BLUEBERRY:
			moveTimer++;
			if (moveTimer > 100)
			{
				moveDirection *= -1.0f;
				moveTimer = 0;
			}
			velocityX = moveDirection * 4.0f * speedMultiplier;
			break;

		case ACORN_MAKER:
			moveTimer++;
			if (moveTimer > 200)
			{
				moveDirection *= -1.0f;
				moveTimer = 0;
			}
			velocityX = moveDirection * 1.8f * speedMultiplier;
			break;
	}

	velocityY += GRAVITY * 0.5f;
	if (velocityY > TERMINAL_VELOCITY * 0.8f)
		velocityY = TERMINAL_VELOCITY * 0.8f;

	sprite.move(velocityX, velocityY);

	if (sprite.getPosition().y + size * 2 >= GROUND_LEVEL)
	{
		sprite.setPosition(sprite.getPosition().x, GROUND_LEVEL - size * 2);
		velocityY = 0;
	}

	if (sprite.getPosition().x < -50)
		active = false;
	if (sprite.getPosition().x > WORLD_WIDTH + 50)
		sprite.setPosition(WORLD_WIDTH, sprite.getPosition().y);
}

void Enemy::takeDamage(int damage)
{
	health -= damage;
	if (health <= 0)
		active = false;
}

void Enemy::draw(RenderWindow& window)
{
	if (!active) return;

	// Draw enemy sprite with texture
	window.draw(sprite);
}


bool Enemy::isActive() const { return active; }
FloatRect Enemy::getBounds() const 
{ 
	return FloatRect(sprite.getPosition().x, sprite.getPosition().y, size * 2, size * 2);
}
Vector2f Enemy::getPosition() const { return sprite.getPosition(); }
int Enemy::getHealth() const { return health; }
int Enemy::getMaxHealth() const { return maxHealth; }
EnemyType Enemy::getType() const { return enemyType; }
