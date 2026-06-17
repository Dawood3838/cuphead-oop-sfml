#include "boss.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Boss::Boss() : health(500), maxHealth(500), isAlive(true), currentFrame(0), 
               animationTimer(0), frameDelay(0.08f), currentAttack(BOSS_ATTACK_LASER),
               attackCooldown(0), attackDuration(0), isAttacking(false), attackPhase(0),
               laserSpawnTimer(0), slamTimer(0), summonTimer(0), tongueTimer(0), stompTimer(0), isPhase2(false)
{
	position = Vector2f(1000, 300);
}

Boss::~Boss()
{
	// Textures are automatically cleaned up when vectors go out of scope
}

bool Boss::loadAnimationFrames(const std::string& folderPath, std::vector<Texture>& frames, int maxFrames)
{
	frames.clear();
	bool loaded = false;
	
	for (int i = 1; i <= maxFrames; i++)
	{
		Texture frame;
		std::stringstream ss;
		ss << folderPath << "/devil_" << std::setfill('0') << std::setw(4) << i << ".png";
		
		if (frame.loadFromFile(ss.str()))
		{
			frames.push_back(frame);
			loaded = true;
		}
		else
		{
			// If we can't find a frame, we've likely found all that exist
			break;
		}
	}
	
	return loaded;
}

void Boss::initialize(float x, float y, bool isPhase2)
{
	position = Vector2f(x, y);
	health = maxHealth;
	isAlive = true;
	currentFrame = 0;
	animationTimer = 0;
	isAttacking = false;
	attackCooldown = 0.1f;  // Start attacking almost IMMEDIATELY!
	
	// Store phase for later use
	this->isPhase2 = isPhase2;
	
	std::string phase = isPhase2 ? "Phase 2" : "Phase 1";
	std::cout << "Boss initialized at (" << x << ", " << y << ") with health: " << health << " (" << phase << ")" << std::endl;
	
	if (isPhase2)
	{
		// PHASE 2 - Different structure with ph3 naming
		std::string assetPath = "Data/Phase 2/";
		
		// Load idle animation frames - ph3 naming
		for (int i = 1; i <= 10; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << "Idle/devil_ph3_idle_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
			{
				idleFrames.push_back(frame);
				std::cout << "Loaded Phase 2 idle frame: " << ss.str() << std::endl;
			}
			else
			{
				break;
			}
		}
		
		// Load attack frames - Axe of Torment (Attack 1)
		for (int i = 1; i <= 30; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << "Attack/Axe of Torment/Attack/devil_ph3_axe_spin_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
			{
				attackFrames[BOSS_ATTACK_SLAM].push_back(frame);
			}
			else
			{
				break;
			}
		}
		
		// Load attack frames - Chips of Greed (Attack 2)
		for (int i = 1; i <= 30; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << "Attack/Chips of Greed/Chip/devil_ph3_chip_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
			{
				attackFrames[BOSS_ATTACK_LASER].push_back(frame);
			}
			else
			{
				break;
			}
		}
		
		// Load attack frames - Demonic Bomb (Attack 3)
		for (int i = 1; i <= 30; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << "Attack/Demonic Bomb/Bomb/devil_ph3_bomb_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
			{
				attackFrames[BOSS_ATTACK_TONGUE].push_back(frame);
			}
			else
			{
				break;
			}
		}
		
		// Reuse remaining attacks with Demonic Bomb variants
		attackFrames[BOSS_ATTACK_SUMMON] = attackFrames[BOSS_ATTACK_TONGUE];
		attackFrames[BOSS_ATTACK_STOMP] = attackFrames[BOSS_ATTACK_LASER];
		
		// Load neck frames for death animation (fallback)
		for (int i = 1; i <= 3; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << "Neck/devil_ph3_neck_boil_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
			{
				deathFrames.push_back(frame);
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		// PHASE 1 - Original structure
		std::string assetPath = "Data/Phase 1/";
		std::string attackPath = "Attacks";
		
		// Load idle animation frames - try Idle folder
		for (int i = 1; i <= 30; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << "Idle/devil_idle_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
				idleFrames.push_back(frame);
			else
				break;
		}
		
		// If no idle frames loaded, try single idle image
		if (idleFrames.empty())
		{
			Texture frame;
			std::string singlePath = assetPath + "Idle/devil_idle.png";
			if (frame.loadFromFile(singlePath))
				idleFrames.push_back(frame);
		}
		
		// Load attack animation frames for each attack type
		// Ram Clap (hand slap)
		for (int i = 1; i <= 30; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << attackPath << "/Ram Clap/devil_ph1_ram_trans_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
				attackFrames[BOSS_ATTACK_SLAM].push_back(frame);
			else
				break;
		}
		
		// Orbs of Pain (laser attack) - in Intro subfolder, only 8 frames
		for (int i = 1; i <= 8; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << attackPath << "/Orbs of Pain/Intro/devil_ph1_trident_spin_body_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
				attackFrames[BOSS_ATTACK_LASER].push_back(frame);
			else
				break;
		}
		
		// Serpent Chaser (pink tongue - parryable) - max 35 frames
		for (int i = 1; i <= 35; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << attackPath << "/Serpent Chaser/devil_ph1_dragon_trans_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
				attackFrames[BOSS_ATTACK_TONGUE].push_back(frame);
			else
				break;
		}
		
		// Demonic Summoning - check Intro folder first, only 27 frames
		for (int i = 1; i <= 27; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << attackPath << "/Demonic Summoning/Intro/devil_ph1_imp_peak_out_a_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
				attackFrames[BOSS_ATTACK_SUMMON].push_back(frame);
			else
				break;
		}
		
		// Spider Stomp
		for (int i = 1; i <= 30; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << attackPath << "/Spider Stomp/devil_ph1_spider_trans_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
				attackFrames[BOSS_ATTACK_STOMP].push_back(frame);
			else
				break;
		}
		
		// Load death animation frames - max 24 frames for hole_jump
		for (int i = 1; i <= 24; i++)
		{
			Texture frame;
			std::stringstream ss;
			ss << assetPath << "Death/devil_ph1_death_hole_jump_" << std::setfill('0') << std::setw(4) << i << ".png";
			if (frame.loadFromFile(ss.str()))
				deathFrames.push_back(frame);
			else
				break;
		}
	}
	
	// Initialize sprite with fallback
	if (!idleFrames.empty())
	{
		sprite.setTexture(idleFrames[0]);
	}
	else
	{
		// Create a placeholder texture
		std::cerr << "Warning: No boss idle frames loaded!" << std::endl;
	}
	
	sprite.setPosition(position);
	sprite.setScale(2.0f, 2.0f);  // Scale boss sprite for visibility
}

void Boss::selectNextAttack()
{
	// Randomly select next attack
	static int lastAttack = -1;
	BossAttackType nextAttack;
	
	do {
		nextAttack = static_cast<BossAttackType>(rand() % 5);
	} while (nextAttack == lastAttack);  // Avoid repeating same attack
	
	lastAttack = nextAttack;
	currentAttack = nextAttack;
	isAttacking = true;
	attackDuration = 0;
	attackPhase = 0;
	
	std::cout << "Boss attacking: " << currentAttack << " (1=SLAM, 2=LASER, 3=SUMMON, 4=TONGUE, 5=STOMP)" << std::endl;
	
	// Reset timers
	laserSpawnTimer = 0;
	slamTimer = 0;
	summonTimer = 0;
	tongueTimer = 0;
	stompTimer = 0;
}

void Boss::updateAttack(float deltaTime, const Vector2f& playerPos)
{
	attackDuration += deltaTime;
	
	// Apply phase 2 multiplier for attack durations (20% faster - attacks end sooner, leading to faster repeats)
	float phaseMult = isPhase2 ? 0.8f : 1.0f;
	
	switch (currentAttack)
	{
		case BOSS_ATTACK_LASER:  // Orbs of Pain
		{
			// Spawn orbs in pattern
			laserSpawnTimer += deltaTime;
			if (laserSpawnTimer > 0.5f && attackDuration < 5.0f * phaseMult)
			{
				laserSpawnTimer = 0;
				// Orbs would be spawned here (handled in main game loop)
			}
			if (attackDuration > 5.0f * phaseMult)
			{
				isAttacking = false;
				attackCooldown = isPhase2 ? 0.4f : 0.5f;  // 20% faster cooldown in phase 2
			}
			break;
		}
		
		case BOSS_ATTACK_SLAM:  // Ram Clap (hand slap)
		{
			// Slam attack - stays attacking for longer
			if (attackDuration > 4.0f * phaseMult)
			{
				isAttacking = false;
				attackCooldown = isPhase2 ? 0.24f : 0.3f;  // 20% faster cooldown in phase 2
			}
			break;
		}
		
		case BOSS_ATTACK_SUMMON:  // Demonic Summoning
		{
			// Summon mobs - long attack duration
			summonTimer += deltaTime;
			if (summonTimer > 0.7f && attackDuration < 6.0f * phaseMult)
			{
				summonTimer = 0;
				// Summon mobs here
			}
			if (attackDuration > 6.0f * phaseMult)
			{
				isAttacking = false;
				attackCooldown = isPhase2 ? 0.4f : 0.5f;  // 20% faster cooldown in phase 2
			}
			break;
		}
		
		case BOSS_ATTACK_TONGUE:  // Serpent Chaser (parryable)
		{
			// Pink tongue attack - player must parry - long duration
			if (attackDuration > 5.0f * phaseMult)
			{
				isAttacking = false;
				attackCooldown = isPhase2 ? 0.24f : 0.3f;  // 20% faster cooldown in phase 2
			}
			break;
		}
		
		case BOSS_ATTACK_STOMP:  // Spider Stomp
		{
			// Heavy stomp attack - stays attacking longer
			if (attackDuration > 4.5f * phaseMult)
			{
				isAttacking = false;
				attackCooldown = isPhase2 ? 0.24f : 0.3f;  // 20% faster cooldown in phase 2
			}
			break;
		}
	}
}

void Boss::update(float deltaTime, const Vector2f& playerPos)
{
	if (!isAlive)
	{
		// Play death animation
		animationTimer += deltaTime;
		if (animationTimer > frameDelay && !deathFrames.empty())
		{
			animationTimer = 0;
			currentFrame++;
			if (currentFrame >= deathFrames.size())
			{
				currentFrame = deathFrames.size() - 1;
			}
			else
			{
				sprite.setTexture(deathFrames[currentFrame]);
			}
		}
		return;
	}
	
	// Handle attack cooldown
	if (!isAttacking)
	{
		attackCooldown -= deltaTime;
		if (attackCooldown <= 0)
		{
			selectNextAttack();
		}
	}
	
	// Update current attack
	if (isAttacking)
	{
		updateAttack(deltaTime, playerPos);
	}
	
	// Update animation
	animationTimer += deltaTime;
	if (animationTimer > frameDelay)
	{
		animationTimer = 0;
		currentFrame++;
		
		if (isAttacking && !attackFrames[currentAttack].empty())
		{
			// Use attack animation
			if (currentFrame >= attackFrames[currentAttack].size())
			{
				currentFrame = 0;
			}
			sprite.setTexture(attackFrames[currentAttack][currentFrame]);
		}
		else if (!idleFrames.empty())
		{
			// Use idle animation
			if (currentFrame >= idleFrames.size())
			{
				currentFrame = 0;
			}
			sprite.setTexture(idleFrames[currentFrame]);
		}
	}
	
	// Update sprite position (boss stays mostly in place, slight movement)
	sprite.setPosition(position);
}

void Boss::draw(RenderWindow& window)
{
	if (isAlive)
	{
		window.draw(sprite);
		
		// Draw health bar
		float healthBarWidth = 300;
		float healthBarHeight = 20;
		float healthPercent = getHealthPercent();
		
		// Background bar (red)
		RectangleShape healthBarBg(Vector2f(healthBarWidth, healthBarHeight));
		healthBarBg.setPosition(position.x - healthBarWidth / 2, position.y - 100);
		healthBarBg.setFillColor(Color::Red);
		window.draw(healthBarBg);
		
		// Health bar (green)
		RectangleShape healthBar(Vector2f(healthBarWidth * healthPercent, healthBarHeight));
		healthBar.setPosition(position.x - healthBarWidth / 2, position.y - 100);
		healthBar.setFillColor(Color::Green);
		window.draw(healthBar);
	}
}

void Boss::takeDamage(float damage)
{
	health -= damage;
	if (health <= 0)
	{
		health = 0;
		isAlive = false;
		currentFrame = 0;
	}
}
