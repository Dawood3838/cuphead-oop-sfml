#ifndef BOSS_H
#define BOSS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>

using namespace sf;

// Boss attack types
enum BossAttackType
{
	BOSS_ATTACK_LASER,      // Orbs of Pain
	BOSS_ATTACK_SLAM,       // Ram Clap (hand slap)
	BOSS_ATTACK_SUMMON,     // Demonic Summoning
	BOSS_ATTACK_TONGUE,     // Serpent Chaser (pink tongue - parryable)
	BOSS_ATTACK_STOMP       // Spider Stomp
};

// Devil Boss Class
class Boss
{
private:
	// Position and state
	Vector2f position;
	float health;
	float maxHealth;
	bool isAlive;
	
	// Animation
	std::vector<Texture> idleFrames;
	std::vector<Texture> attackFrames[5];  // For each attack type
	std::vector<Texture> deathFrames;
	int currentFrame;
	float animationTimer;
	float frameDelay;
	
	// Attack system
	BossAttackType currentAttack;
	float attackCooldown;
	float attackDuration;
	bool isAttacking;
	int attackPhase;
	
	// Attack timers
	float laserSpawnTimer;
	float slamTimer;
	float summonTimer;
	float tongueTimer;
	float stompTimer;
	
	// Sprite rendering
	Sprite sprite;
	
	// Phase tracking
	bool isPhase2;
	
	// Loading helpers
	bool loadAnimationFrames(const std::string& folderPath, std::vector<Texture>& frames, int maxFrames = 30);
	
public:
	Boss();
	~Boss();
	
	// Initialization
	void initialize(float x, float y, bool isPhase2 = false);
	
	// Update and render
	void update(float deltaTime, const Vector2f& playerPos);
	void draw(RenderWindow& window);
	
	// Attack management
	void selectNextAttack();
	void updateAttack(float deltaTime, const Vector2f& playerPos);
	
	// Health management
	void takeDamage(float damage);
	bool isDead() const { return !isAlive; }
	float getHealth() const { return health; }
	float getHealthPercent() const { return health / maxHealth; }
	
	// Getters
	Vector2f getPosition() const { return position; }
	FloatRect getBounds() const { return sprite.getGlobalBounds(); }
	
	// Attack query
	bool isCurrentlyAttacking() const { return isAttacking; }
	BossAttackType getCurrentAttack() const { return currentAttack; }
};

#endif
