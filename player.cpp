#include "player.h"
#include <cmath>
#include <iostream>

Player::Player() : velocityY(0), onGround(false), canDoubleJump(false), currentCharacter(nullptr), 
                   currentCharType(CUPHEAD), walkAnimationFrame(0), walkAnimationTimer(0),
                   currentAnimationFrame(0), animationTimer(0),
                   superMeter(0), isParrying(false), parryTimer(0), superActive(false), superTimer(0),
                   facingDirection(1), takingDamage(false), damageTimer(0), knockbackX(0), knockbackY(0),
                   charSwitchCooldown(0), isMoving(false)
{
	cuphead.resetHealth();
	mugman.resetHealth();
	currentCharacter = &cuphead;
	currentCharType = CUPHEAD;
}

void Player::initialize()
{
	sprite.setPosition(500, 150);
	
	// ==================== LOAD CUPHEAD ANIMATIONS ====================
	// Load idle texture from Normal folder
	if (!cupheadIdleTexture.loadFromFile("Data/Normal/player.png"))
	{
		cupheadIdleTexture.loadFromFile("Data/player.png");  // Fallback
	}
	
	// Load Cuphead animation frames from Normal folder (15 frames)
	for (int i = 1; i <= 15; i++)
	{
		sf::Texture frame;
		std::string framePath = "Data/Normal/cuphead_run_";
		if (i < 10)
			framePath += "000";
		else
			framePath += "00";
		framePath += std::to_string(i) + ".png";
		
		if (frame.loadFromFile(framePath))
		{
			cupheadAnimationFrames.push_back(frame);
		}
	}
	
	// ==================== LOAD MUGMAN ANIMATIONS ====================
	// Load idle texture from Walking folder (with transparent background)
	if (!mugmanIdleTexture.loadFromFile("Data/Walking/mugback.png"))
	{
		mugmanIdleTexture.loadFromFile("Data/mugfinal.png");  // Fallback
	}
	
	// Load Mugman animation frames from Walking folder (16 frames)
	// Filenames: mugman_run_001.png (1-9), mugman_run_0010.png (10-16)
	for (int i = 1; i <= 16; i++)
	{
		sf::Texture frame;
		std::string framePath = "Data/Walking/mugman_run_";
		if (i < 10)
		{
			framePath += "00" + std::to_string(i);
		}
		else
		{
			framePath += "00" + std::to_string(i);
		}
		framePath += ".png";
		
		if (frame.loadFromFile(framePath))
		{
			mugmanAnimationFrames.push_back(frame);
		}
	}
	
	// Set initial character to Cuphead with proper initialization
	sprite.setTexture(cupheadIdleTexture);
	currentAnimationFrame = 0;
	isMoving = false;
	
	sprite.setScale(0.75f, 0.75f);
	speed = currentCharacter->getSpeed();
}

void Player::switchCharacter()
{
	if (currentCharType == CUPHEAD)
	{
		currentCharacter = &mugman;
		currentCharType = MUGMAN;
		// Switch to Mugman idle texture and animations
		sprite.setTexture(mugmanIdleTexture);
		currentAnimationFrame = 0;
	}
	else
	{
		currentCharacter = &cuphead;
		currentCharType = CUPHEAD;
		// Switch to Cuphead idle texture and animations
		sprite.setTexture(cupheadIdleTexture);
		currentAnimationFrame = 0;
	}
	speed = currentCharacter->getSpeed();
	isMoving = false;
	charSwitchCooldown = 0.5f;
}

void Player::update()
{
	// Update animation frames only when moving (cycle through frames - faster speed)
	if (currentCharType == CUPHEAD && !cupheadAnimationFrames.empty() && isMoving)
	{
		animationTimer += 0.016f;
		if (animationTimer > 0.06f)  // Change frame every 0.06 seconds (faster animation)
		{
			animationTimer = 0;
			currentAnimationFrame = (currentAnimationFrame + 1) % cupheadAnimationFrames.size();
			sprite.setTexture(cupheadAnimationFrames[currentAnimationFrame]);
		}
	}
	else if (currentCharType == MUGMAN && !mugmanAnimationFrames.empty() && isMoving)
	{
		animationTimer += 0.016f;
		if (animationTimer > 0.06f)  // Change frame every 0.06 seconds (faster animation)
		{
			animationTimer = 0;
			currentAnimationFrame = (currentAnimationFrame + 1) % mugmanAnimationFrames.size();
			sprite.setTexture(mugmanAnimationFrames[currentAnimationFrame]);
		}
	}
	else if (!isMoving)
	{
		// Show idle texture when not moving
		if (animationTimer > 0 || currentAnimationFrame != 0)
		{
			animationTimer = 0;
			currentAnimationFrame = 0;
			if (currentCharType == CUPHEAD)
				sprite.setTexture(cupheadIdleTexture);
			else
				sprite.setTexture(mugmanIdleTexture);
		}
	}
	
	// Reset movement flag each frame
	isMoving = false;
	
	walkAnimationTimer += 0.016f;
	if (walkAnimationTimer > 0.1f)
	{
		walkAnimationTimer = 0;
		walkAnimationFrame = (walkAnimationFrame + 1) % 4;
	}

	if (isParrying)
	{
		parryTimer -= 0.016f;
		if (parryTimer <= 0)
			isParrying = false;
	}

	if (superActive)
	{
		superTimer -= 0.016f;
		if (superTimer <= 0)
			superActive = false;
	}

	if (takingDamage)
	{
		damageTimer -= 0.016f;
		if (damageTimer <= 0)
			takingDamage = false;
		sprite.move(knockbackX * 0.016f, knockbackY * 0.016f);
		knockbackX *= 0.9f;
		knockbackY *= 0.9f;
	}

	if (!onGround)
	{
		velocityY += GRAVITY;
		if (velocityY > TERMINAL_VELOCITY)
			velocityY = TERMINAL_VELOCITY;
	}
	else
	{
		velocityY = 0;
		canDoubleJump = false;
	}

	sprite.move(0, velocityY);

	// Ground collision with better precision
	if (sprite.getPosition().y + 76 >= GROUND_LEVEL)
	{
		sprite.setPosition(sprite.getPosition().x, GROUND_LEVEL - 76);
		onGround = true;
		canDoubleJump = false;
	}
	else
		onGround = false;

	if (sprite.getPosition().x < 0)
		sprite.setPosition(0, sprite.getPosition().y);
	if (sprite.getPosition().x > WORLD_WIDTH - 72)
		sprite.setPosition(WORLD_WIDTH - 72, sprite.getPosition().y);
}

void Player::handleInput(float dirX, float dirY)
{
	if (dirX < 0)
		moveLeft();
	else if (dirX > 0)
		moveRight();
}

void Player::moveLeft() 
{ 
	sprite.move(-speed, 0);
	walkAnimationFrame = (walkAnimationFrame + 1) % 4;
	facingDirection = -1;
	isMoving = true;
}

void Player::moveRight() 
{ 
	sprite.move(speed, 0);
	walkAnimationFrame = (walkAnimationFrame + 1) % 4;
	facingDirection = 1;
	isMoving = true;
}

void Player::jump()
{
	if (onGround)
	{
		velocityY = -currentCharacter->getJumpPower();
		onGround = false;
		canDoubleJump = true;
	}
	else if (canDoubleJump && !isParrying)
	{
		velocityY = -currentCharacter->getJumpPower() * 0.8f;
		canDoubleJump = false;
		isParrying = true;
		parryTimer = 0.5f;
	}
}

void Player::takeDamage(int damage)
{
	currentCharacter->takeDamage(damage);
	takingDamage = true;
	damageTimer = 0.5f;
	knockbackX = -facingDirection * 200.0f;
	knockbackY = -150.0f;
}

void Player::heal(int amount)
{
	currentCharacter->heal(amount);
}

void Player::increaseMeter(float amount)
{
	superMeter += amount;
	if (superMeter > 100.0f)
		superMeter = 100.0f;
}

void Player::activateSuper()
{
	if (superMeter >= 100.0f && !superActive)
	{
		superMeter = 0;
		superActive = true;
		superTimer = 3.0f;  // Super lasts 3 seconds
	}
}

void Player::draw(RenderWindow& window)
{
	Vector2f playerPos = sprite.getPosition();
	
	Color charColor = currentCharacter->getColor();
	
	if (takingDamage)
	{
		float blinkAlpha = sin(damageTimer * 20.0f) * 0.5f + 0.5f;
		if (blinkAlpha < 0.3f) blinkAlpha = 0.3f;
		charColor.a = static_cast<sf::Uint8>(blinkAlpha * 255);
		
		RectangleShape hitFlash(Vector2f(72, 76));
		hitFlash.setPosition(playerPos);
		hitFlash.setFillColor(Color(255, 100, 100, static_cast<sf::Uint8>(blinkAlpha * 100)));
		window.draw(hitFlash);
	}
	
	sprite.setColor(charColor);
	window.draw(sprite);
	sprite.setColor(Color::White);

	drawRunningAnimation(window, playerPos);

	if (superActive)
	{
		CircleShape aura(45);
		aura.setPosition(playerPos - Vector2f(45, 45));
		aura.setFillColor(Color(255, 215, 0, 100));
		window.draw(aura);
	}

	if (isParrying)
	{
		CircleShape parryIndicator(15);
		parryIndicator.setPosition(playerPos + Vector2f(15, -25));
		parryIndicator.setFillColor(Color(255, 192, 203));
		window.draw(parryIndicator);
	}

	RectangleShape healthBar(Vector2f(100, 10));
	healthBar.setPosition(10, 10);
	healthBar.setFillColor(Color::Black);
	window.draw(healthBar);

	RectangleShape healthFill(Vector2f((float)currentCharacter->getHealth(), 10));
	healthFill.setPosition(10, 10);
	healthFill.setFillColor(Color::Green);
	window.draw(healthFill);

	RectangleShape superMeterBg(Vector2f(100, 8));
	superMeterBg.setPosition(10, 25);
	superMeterBg.setFillColor(Color::Black);
	window.draw(superMeterBg);

	RectangleShape superMeterFill(Vector2f(superMeter, 8));
	superMeterFill.setPosition(10, 25);
	superMeterFill.setFillColor(Color::Magenta);
	window.draw(superMeterFill);

	RectangleShape charIndicator(Vector2f(120, 25));
	charIndicator.setPosition(10, 30);
	charIndicator.setFillColor(Color(50, 50, 50, 200));
	window.draw(charIndicator);
}

void Player::drawRunningAnimation(RenderWindow& window, Vector2f playerPos)
{
	if (!onGround) return;

	RectangleShape leg1(Vector2f(4, 12));
	RectangleShape leg2(Vector2f(4, 12));
	
	leg1.setFillColor(currentCharacter->getColor());
	leg2.setFillColor(currentCharacter->getColor());

	float legOffset = 8.0f;
	float legBaseY = playerPos.y + 60;
	float legBaseX = playerPos.x + 30;

	if (walkAnimationFrame == 0)
	{
		leg1.setPosition(legBaseX - legOffset, legBaseY + 8);
		leg2.setPosition(legBaseX + legOffset, legBaseY);
	}
	else if (walkAnimationFrame == 1)
	{
		leg1.setPosition(legBaseX - legOffset, legBaseY + 4);
		leg2.setPosition(legBaseX + legOffset, legBaseY + 4);
	}
	else if (walkAnimationFrame == 2)
	{
		leg1.setPosition(legBaseX - legOffset, legBaseY);
		leg2.setPosition(legBaseX + legOffset, legBaseY + 8);
	}
	else
	{
		leg1.setPosition(legBaseX - legOffset, legBaseY + 4);
		leg2.setPosition(legBaseX + legOffset, legBaseY + 4);
	}

	window.draw(leg1);
	window.draw(leg2);
}

void Player::applyKnockback(int knockbackDirection)
{
	takingDamage = true;
	damageTimer = 0.5f;
	knockbackX = -knockbackDirection * 200.0f;
	knockbackY = -150.0f;
}

FloatRect Player::getHitbox() const 
{ 
	return FloatRect(sprite.getPosition().x + 10, sprite.getPosition().y + 20, 52, 56); 
}

FloatRect Player::getHurtbox() const 
{ 
	return FloatRect(sprite.getPosition().x, sprite.getPosition().y, 72, 76); 
}

FloatRect Player::getBounds() const 
{ 
	return getHurtbox();
}

Vector2f Player::getPosition() const { return sprite.getPosition(); }
int Player::getHealth() const { return currentCharacter->getHealth(); }
int Player::getMaxHealth() const { return currentCharacter->getMaxHealth(); }
bool Player::isOnGround() const { return onGround; }
void Player::setOnGround(bool ground) { onGround = ground; }
void Player::setVelocityY(float vel) { velocityY = vel; }

float Player::getSuperMeter() const { return superMeter; }
bool Player::isSuperActive() const { return superActive; }
bool Player::isInParryState() const { return isParrying; }
CharacterType Player::getCharacterType() const { return currentCharType; }
string Player::getCharacterName() const 
{ 
	if (currentCharType == CUPHEAD)
		return "Cuphead";
	else
		return "Mugman";
}
void Player::resetHealth() { currentCharacter->resetHealth(); }

void Player::setPosition(Vector2f pos)
{
	sprite.setPosition(pos);
}

void Player::setState(Vector2f pos, float velY, bool onGnd, int charType, int health, int weapon)
{
	sprite.setPosition(pos);
	velocityY = velY;
	onGround = onGnd;
	canDoubleJump = !onGnd;
	
	// Set character if needed
	if (charType != currentCharType)
	{
		switchCharacter();
	}
	
	// Restore health
	if (charType == CUPHEAD)
	{
		cuphead.heal(100);  // Reset to max
		while (cuphead.getHealth() > health)
			cuphead.takeDamage(1);
	}
	else
	{
		mugman.heal(120);  // Reset to max
		while (mugman.getHealth() > health)
			mugman.takeDamage(1);
	}
}

Vector2f Player::getVelocity() const
{
	return Vector2f(0, velocityY);
}

float Player::getVelocityY() const
{
	return velocityY;
}
