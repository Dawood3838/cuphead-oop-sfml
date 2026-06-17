#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "character.h"
#include "constants.h"

using namespace sf;

// ==================== PLAYER CLASS ====================
class Player
{
private:
	Sprite sprite;
	Texture texture;
	Texture cupheadTexture;
	Texture mugmanTexture;
	float velocityY;
	bool onGround;
	bool canDoubleJump;
	float speed;
	Character* currentCharacter;
	Cuphead cuphead;
	Mugman mugman;
	CharacterType currentCharType;
	int walkAnimationFrame;
	float walkAnimationTimer;
	std::vector<sf::Texture> cupheadAnimationFrames;
	std::vector<sf::Texture> mugmanAnimationFrames;
	sf::Texture cupheadIdleTexture;
	sf::Texture mugmanIdleTexture;
	int currentAnimationFrame;
	float animationTimer;
	bool isMoving;
	float superMeter;
	bool isParrying;
	float parryTimer;
	bool superActive;
	float superTimer;
	int facingDirection;
	bool takingDamage;
	float damageTimer;
	float knockbackX;
	float knockbackY;
	float charSwitchCooldown;

public:
	Player();

	void initialize();
	void switchCharacter();
	void update();
	void handleInput(float dirX, float dirY);
	void moveLeft();
	void moveRight();
	void jump();
	void takeDamage(int damage);
	void heal(int amount);
	void increaseMeter(float amount);
	void activateSuper();
	void draw(RenderWindow& window);
	void drawRunningAnimation(RenderWindow& window, Vector2f playerPos);
	void applyKnockback(int knockbackDirection);

	FloatRect getHitbox() const;
	FloatRect getHurtbox() const;
	FloatRect getBounds() const;
	Vector2f getPosition() const;
	int getHealth() const;
	int getMaxHealth() const;
	bool isOnGround() const;
	void setOnGround(bool ground);
	void setVelocityY(float vel);
	float getSuperMeter() const;
	bool isSuperActive() const;
	bool isInParryState() const;
	CharacterType getCharacterType() const;
	string getCharacterName() const;
	void resetHealth();
	
	// State getters/setters for rewind
	void setPosition(Vector2f pos);
	void setState(Vector2f pos, float velY, bool onGnd, int charType, int health, int weapon);
	Vector2f getVelocity() const;
	float getVelocityY() const;
};

#endif // PLAYER_H
