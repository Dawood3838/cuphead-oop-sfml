#include "constants.h"
#include "character.h"
#include "camera.h"
#include "projectile.h"
#include "enemy.h"
#include "player.h"
#include "level.h"
#include "menu.h"
#include "boss.h"
#include "gamestate.h"
#include "savegame.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>

using namespace sf;
using namespace std;

class GameManager
{
private:
	Player player;
	Enemy enemies[MAX_ENEMIES];
	Projectile projectiles[MAX_PROJECTILES];
	Platform platforms[MAX_PLATFORMS];
	SpawnPoint spawnPoints[MAX_SPAWN_POINTS];
	GoalFlag goalFlag;
	Camera camera;
	int score;
	int enemyCount;
	int projectileCount;
	int platformCount;
	int spawnPointCount;
	RenderWindow& window;
	Font font;
	Text scoreText;
	bool fontLoaded;
	WeaponType currentWeapon;
	float shootCooldown;
	int lives;
	float collisionCooldown;
	int enemiesKilled;
	float spawnTimer;
	float spawnCooldown;
	bool levelComplete;
	int levelScore;
	Texture forestTexture;
	Sprite forestSprite;
	Texture bossTexture;
	Sprite bossSprite;
	Texture phase2Texture;
	Sprite phase2Sprite;
	float cKeyPressTime;
	int levelType;
	Texture shortPlatformTexture;
	Texture bigPlatformTexture;
	bool shortPlatformLoaded;
	bool bigPlatformLoaded;
	Boss boss;
	bool bossLevel;
	float bossCollisionCooldown;
	GameStateManager gameStateManager;
	SaveGameManager saveGameManager;
	float rewindTimer;
	float maxRewindTime;
	bool isPaused;
	bool showPauseMenu;
	float levelElapsedTime;
	bool shouldExitToMenu;
	int currentSaveSlot;  // Current slot to save to (0-4)
	int bossPhase;  // 1 or 2 for boss phases
	float phaseScalingMultiplier;  // Player damage multiplier (0.8 for phase 2, 1.0 for phase 1)
	float enemyScalingMultiplier;  // Enemy speed/attack multiplier (1.0 for phase 1, 1.2 for phase 2)
	bool showingPhase2Transition;  // Flag to show Phase 2 transition screen
	float phase2TransitionTimer;  // Timer for Phase 2 transition screen
	sf::Music backgroundMusic;  // Background music for looping

public:
	GameManager(RenderWindow& w, int level = 0) : score(0), enemyCount(0), projectileCount(0), platformCount(0), spawnPointCount(0),
	                                 window(w), fontLoaded(false), currentWeapon(PEASHOOTER), shootCooldown(0), 
	                                 lives(3), collisionCooldown(-1.0f), enemiesKilled(0), spawnTimer(0), 
	                                 spawnCooldown(2.0f), levelComplete(false), levelScore(0), cKeyPressTime(0), 
	                                 levelType(level), shortPlatformLoaded(false), bigPlatformLoaded(false), bossLevel(level == 1), bossCollisionCooldown(0),
	                                 rewindTimer(0), maxRewindTime(10.0f), isPaused(false), showPauseMenu(false), levelElapsedTime(0), shouldExitToMenu(false), currentSaveSlot(0),
	                                 bossPhase(1), phaseScalingMultiplier(1.0f), enemyScalingMultiplier(1.0f), showingPhase2Transition(false), phase2TransitionTimer(0)
	{
		player.initialize();
		loadPlatformTextures();
		
		if (levelType == 0)
		{
			initializeLevelForest();
		}
		else
		{
			initializeLevelBoss();
			boss.initialize(1400, 250);  // Initialize boss at specific position
		}
		
		initializeEnemies();
		initializeMusic();
		initializeFont();
		initializeBackground();
	}

	void initializeBackground()
	{
		// Load forest background
		if (forestTexture.loadFromFile("Data/forest.png"))
		{
			forestSprite.setTexture(forestTexture);
			forestSprite.setPosition(0, 0);
			// Scale to cover screen height
			float scaleY = (float)SCREEN_Y / forestTexture.getSize().y;
			forestSprite.setScale(1.0f, scaleY);
		}
		
		// Load boss background (devilfinal.png)
		if (bossTexture.loadFromFile("Data/devilfinal.png"))
		{
			bossSprite.setTexture(bossTexture);
			bossSprite.setPosition(0, 0);
			// Scale to cover screen height (1980x1080 image, scale height to 896)
			float scaleY = (float)SCREEN_Y / bossTexture.getSize().y;
			bossSprite.setScale(1.0f, scaleY);
		}
		
		// Load Phase 2 background (phase2back.png)
		if (phase2Texture.loadFromFile("Data/phase2back.png"))
		{
			phase2Sprite.setTexture(phase2Texture);
			phase2Sprite.setPosition(0, 0);
			// Scale to cover screen height
			float scaleY = (float)SCREEN_Y / phase2Texture.getSize().y;
			phase2Sprite.setScale(1.0f, scaleY);
			std::cout << "Phase 2 background loaded successfully!" << std::endl;
		}
		else
		{
			std::cerr << "Warning: Could not load Phase 2 background (phase2back.png)" << std::endl;
		}
	}

	void loadPlatformTextures()
	{
		// Load short platform texture
		if (shortPlatformTexture.loadFromFile("Data/shortplatform.png"))
		{
			shortPlatformLoaded = true;
		}
		
		// Load big platform texture
		if (bigPlatformTexture.loadFromFile("Data/bigplatform.png"))
		{
			bigPlatformLoaded = true;
		}
	}

	void initializeMusic()
	{
		// Load and play background music
		if (backgroundMusic.openFromFile("Data/mus.ogg"))
		{
			backgroundMusic.setLoop(true);  // Enable looping
			backgroundMusic.play();
			std::cout << "Background music loaded and playing" << std::endl;
		}
		else
		{
			std::cout << "Warning: Could not load background music (Data/mus.ogg)" << std::endl;
		}
	}

	void initializeFont()
	{
		// Try to load a system font - if it fails, we'll use default rendering
		// Font paths vary by system, but we'll try common locations
		if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
		{
			if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"))
			{
				fontLoaded = false;
				return;
			}
		}
		fontLoaded = true;
		scoreText.setFont(font);
		scoreText.setCharacterSize(30);
		scoreText.setFillColor(Color::White);
		scoreText.setPosition(SCREEN_X - 250, 10);
	}

	void initializeLevelForest()
	{
		// FOREST FOLLIES Level Layout with textured platforms
		platformCount = 0;

		// Ground level platform
		platforms[platformCount].initialize(0, GROUND_LEVEL, WORLD_WIDTH, SCREEN_Y - GROUND_LEVEL, Color(139, 69, 19));
		platformCount++;

		// Starting area - gentle slope
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(100, GROUND_LEVEL - 50, 200, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(100, GROUND_LEVEL - 50, 200, 50, Color::Green);
		platformCount++;

		// First jump section
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(400, GROUND_LEVEL - 100, 150, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(400, GROUND_LEVEL - 100, 150, 50, Color::Green);
		platformCount++;
		
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(650, GROUND_LEVEL - 80, 150, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(650, GROUND_LEVEL - 80, 150, 50, Color::Green);
		platformCount++;

		// Mid-level platforms (forest trees)
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(900, GROUND_LEVEL - 200, 200, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(900, GROUND_LEVEL - 200, 180, 50, Color(34, 139, 34));
		platformCount++;
		
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1200, GROUND_LEVEL - 150, 200, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(1200, GROUND_LEVEL - 150, 200, 50, Color::Green);
		platformCount++;

		// Climbing section
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1500, GROUND_LEVEL - 120, 100, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(1500, GROUND_LEVEL - 120, 100, 50, Color::Green);
		platformCount++;
		
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1700, GROUND_LEVEL - 200, 100, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(1700, GROUND_LEVEL - 200, 100, 50, Color::Green);
		platformCount++;
		
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1900, GROUND_LEVEL - 250, 100, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(1900, GROUND_LEVEL - 250, 100, 50, Color::Green);
		platformCount++;

		// High platform section
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(2100, GROUND_LEVEL - 300, 250, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(2100, GROUND_LEVEL - 300, 250, 50, Color(34, 139, 34));
		platformCount++;

		// Drop and jump section
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(2500, GROUND_LEVEL - 100, 150, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(2500, GROUND_LEVEL - 100, 150, 50, Color::Green);
		platformCount++;
		
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(2800, GROUND_LEVEL - 150, 150, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(2800, GROUND_LEVEL - 150, 150, 50, Color::Green);
		platformCount++;

		// Long jump challenge
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(3200, GROUND_LEVEL - 80, 200, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(3200, GROUND_LEVEL - 80, 200, 50, Color::Green);
		platformCount++;
		
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(3600, GROUND_LEVEL - 150, 200, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(3600, GROUND_LEVEL - 150, 200, 50, Color::Green);
		platformCount++;

		// Final ascent
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(4000, GROUND_LEVEL - 200, 150, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(4000, GROUND_LEVEL - 200, 150, 50, Color(34, 139, 34));
		platformCount++;
		
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(4250, GROUND_LEVEL - 150, 150, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(4250, GROUND_LEVEL - 150, 150, 50, Color::Green);
		platformCount++;

		// Goal flag platform
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(4600, GROUND_LEVEL - 200, 200, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(4600, GROUND_LEVEL - 200, 200, 50, Color(255, 215, 0));
		platformCount++;

		// Initialize goal flag at the end of level
		goalFlag.initialize(4700, GROUND_LEVEL - 250);

		// Initialize spawn points for enemies at strategic locations
		spawnPointCount = 0;

		// Spawn points along the level - enemies emerge from right side
		spawnPoints[spawnPointCount].initialize(800, 250, DEADLY_DAISY);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(1400, 300, AGGRAVATING_ACORN);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(2000, 200, MURDEROUS_MUSHROOM);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(2600, 350, TERRIBLE_TULIP);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(3300, 250, BOTHERSOME_BLUEBERRY);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(4000, 200, TOOTHY_TERROR);
		spawnPointCount++;
	}

	void initializeLevelBoss()
	{
		// BOSS LEVEL Layout - Shorter level with challenging platforms
		platformCount = 0;

		// Ground level platform
		platforms[platformCount].initialize(0, GROUND_LEVEL, WORLD_WIDTH, SCREEN_Y - GROUND_LEVEL, Color(139, 69, 19));
		platformCount++;

		// Starting area
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(100, GROUND_LEVEL - 50, 250, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(100, GROUND_LEVEL - 50, 250, 50, Color::Green);
		platformCount++;

		// First challenge - narrow platforms
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(500, GROUND_LEVEL - 100, 100, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(500, GROUND_LEVEL - 100, 100, 50, Color::Green);
		platformCount++;
		
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(700, GROUND_LEVEL - 80, 100, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(700, GROUND_LEVEL - 80, 100, 50, Color::Green);
		platformCount++;

		// Mid section - wider platforms
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1000, GROUND_LEVEL - 200, 250, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(1000, GROUND_LEVEL - 200, 250, 50, Color::Green);
		platformCount++;

		// Challenge section - mix of sizes
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1400, GROUND_LEVEL - 150, 120, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(1400, GROUND_LEVEL - 150, 120, 50, Color::Green);
		platformCount++;
		
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1700, GROUND_LEVEL - 100, 220, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(1700, GROUND_LEVEL - 100, 220, 50, Color::Green);
		platformCount++;

		// Boss arena approach
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(2100, GROUND_LEVEL - 200, 100, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(2100, GROUND_LEVEL - 200, 100, 50, Color::Green);
		platformCount++;
		
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(2350, GROUND_LEVEL - 150, 250, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(2350, GROUND_LEVEL - 150, 250, 50, Color::Green);
		platformCount++;

		// Boss arena platform
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(2700, GROUND_LEVEL - 200, 300, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(2700, GROUND_LEVEL - 200, 300, 50, Color(255, 215, 0));
		platformCount++;

		// Initialize goal flag at the end of level
		goalFlag.initialize(2850, GROUND_LEVEL - 250);

		// Initialize spawn points for boss enemies
		spawnPointCount = 0;

		spawnPoints[spawnPointCount].initialize(600, 250, ACORN_MAKER);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(1200, 300, TOOTHY_TERROR);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(1800, 200, ACORN_MAKER);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(2400, 350, TOOTHY_TERROR);
		spawnPointCount++;
	}

	void initializeLevelPhase2()
	{
		// PHASE 2 - Harder version of boss level with increased difficulty
		// Reuse the same level layout from Phase 1
		platformCount = 0;

		// Ground level platform
		platforms[platformCount].initialize(0, GROUND_LEVEL, WORLD_WIDTH, SCREEN_Y - GROUND_LEVEL, Color(139, 69, 19));
		platformCount++;

		// Starting area
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(100, GROUND_LEVEL - 50, 250, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(100, GROUND_LEVEL - 50, 250, 50, Color::Green);
		platformCount++;

		// First challenge - narrow platforms
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(500, GROUND_LEVEL - 100, 100, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(500, GROUND_LEVEL - 100, 100, 50, Color::Green);
		platformCount++;
		
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(700, GROUND_LEVEL - 80, 100, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(700, GROUND_LEVEL - 80, 100, 50, Color::Green);
		platformCount++;

		// Mid section - wider platforms
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1000, GROUND_LEVEL - 200, 250, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(1000, GROUND_LEVEL - 200, 250, 50, Color::Green);
		platformCount++;

		// Challenge section - mix of sizes
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1400, GROUND_LEVEL - 150, 120, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(1400, GROUND_LEVEL - 150, 120, 50, Color::Green);
		platformCount++;
		
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(1700, GROUND_LEVEL - 100, 220, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(1700, GROUND_LEVEL - 100, 220, 50, Color::Green);
		platformCount++;

		// Boss arena approach
		if (shortPlatformLoaded)
			platforms[platformCount].initializeWithTexture(2100, GROUND_LEVEL - 200, 100, 50, &shortPlatformTexture);
		else
			platforms[platformCount].initialize(2100, GROUND_LEVEL - 200, 100, 50, Color::Green);
		platformCount++;
		
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(2350, GROUND_LEVEL - 150, 250, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(2350, GROUND_LEVEL - 150, 250, 50, Color::Green);
		platformCount++;

		// Boss arena platform
		if (bigPlatformLoaded)
			platforms[platformCount].initializeWithTexture(2700, GROUND_LEVEL - 200, 300, 50, &bigPlatformTexture);
		else
			platforms[platformCount].initialize(2700, GROUND_LEVEL - 200, 300, 50, Color(255, 215, 0));
		platformCount++;

		// Initialize goal flag at the end of level
		goalFlag.initialize(2850, GROUND_LEVEL - 250);

		// Initialize spawn points for phase 2 - increased difficulty
		spawnPointCount = 0;

		spawnPoints[spawnPointCount].initialize(600, 250, TOOTHY_TERROR);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(1200, 300, ACORN_MAKER);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(1800, 200, TOOTHY_TERROR);
		spawnPointCount++;
		spawnPoints[spawnPointCount].initialize(2400, 350, ACORN_MAKER);
		spawnPointCount++;

		// Reinitialize boss with Phase 2 assets
		boss = Boss();
		boss.initialize(1400, 250, true);  // true = phase 2
	}

	void initializeEnemies()
	{
		enemies[0].initialize(1200, 200, DEADLY_DAISY, enemyScalingMultiplier);
		enemies[1].initialize(1400, 250, AGGRAVATING_ACORN, enemyScalingMultiplier);
		enemies[2].initialize(1600, 200, DEADLY_DAISY, enemyScalingMultiplier);
		enemyCount = 3;
	}

	void update()
	{
		// Handle Phase 2 transition screen
		if (showingPhase2Transition)
		{
			phase2TransitionTimer += 0.016f;
			if (phase2TransitionTimer > 3.0f)  // Show for 3 seconds
			{
				// Transition to Phase 2
				std::cout << "Transitioning to Phase 2 now..." << std::endl;
				bossPhase = 2;
				phaseScalingMultiplier = 0.8f;  // Player damage reduced by 20%
				enemyScalingMultiplier = 1.2f;  // Enemy speed/attack increased by 20%
				
				// Reset player position to start of level
				player.setPosition(Vector2f(150, GROUND_LEVEL - 100));
				player.setVelocityY(0);
				player.setOnGround(true);
				
				// Reinitialize level for Phase 2
				initializeLevelPhase2();
				
				// Reset goal flag so it can be reached again
				goalFlag.setReached(false);
				
				// Hide transition screen
				showingPhase2Transition = false;
			}
			return;  // Skip normal update during transition
		}
		
		// Update rewind timer
		if (!isPaused)
		{
			rewindTimer += 0.016f;
			levelElapsedTime += 0.016f;
			
			// Record snapshot every 0.01 seconds
			gameStateManager.updateTimer(0.016f);
			if (gameStateManager.shouldSnapshot())
			{
				recordGameState();
				gameStateManager.resetTimer();
			}
		}

		// Don't update game if paused (except rewind timer)
		if (isPaused)
		{
			return;
		}

		player.update();

		// Update boss if in boss level
		if (bossLevel && !boss.isDead())
		{
			boss.update(0.016f, player.getPosition());
		}

		// Update platforms
		for (int i = 0; i < platformCount; i++)
		{
			platforms[i].update();
		}

		// Update goal flag
		goalFlag.update();

		// Update enemies
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			if (enemies[i].isActive())
			{
				enemies[i].update();
			}
		}

		// Update projectiles
		for (int i = 0; i < MAX_PROJECTILES; i++)
		{
			if (projectiles[i].isActive())
			{
				projectiles[i].update();
			}
		}

		// Update camera
		camera.update(player.getPosition());

		// Check if player reached goal flag
		if (!levelComplete && !showingPhase2Transition && goalFlag.checkCollision(player.getHurtbox()))
		{
			goalFlag.setReached(true);
			
			// If in boss level, check if we need to transition to Phase 2
			if (bossLevel && bossPhase == 1 && boss.isDead())
			{
				// Show Phase 2 transition screen
				std::cout << "Phase 1 boss defeated! Showing Phase 2 transition..." << std::endl;
				showingPhase2Transition = true;
				phase2TransitionTimer = 0;
			}
			else if (bossLevel && bossPhase == 2 && boss.isDead())
			{
				// Phase 2 boss defeated - end the game
				levelComplete = true;
				score += 500;  // Bonus for completing level
			}
			else if (!bossLevel)
			{
				// Forest level - just complete
				levelComplete = true;
				score += 500;  // Bonus for completing level
			}
		}

		handleCollisions();
		spawnProgressiveEnemies();
	}

	void spawnProgressiveEnemies()
	{
		spawnTimer += 0.016f;

		// Determine spawn cooldown and enemy types based on progress
		float baseCooldown = 3.0f - (enemiesKilled / 20.0f);  // Faster spawns as you progress
		if (baseCooldown < 0.5f) baseCooldown = 0.5f;

		if (spawnTimer >= baseCooldown)
		{
			spawnTimer = 0;

			// Find an inactive slot
			for (int i = 0; i < MAX_ENEMIES; i++)
			{
				if (!enemies[i].isActive())
				{
					float spawnX = camera.getCameraX() + SCREEN_X + 150;
					float spawnY = 150 + (rand() % 200);

					// Choose enemy type based on progress
					EnemyType typeToSpawn = DEADLY_DAISY;

					if (enemiesKilled < 3)
						typeToSpawn = (rand() % 2 == 0) ? DEADLY_DAISY : AGGRAVATING_ACORN;
					else if (enemiesKilled < 8)
						typeToSpawn = (EnemyType)(DEADLY_DAISY + (rand() % 3));  // Tier 1-3
					else if (enemiesKilled < 15)
						typeToSpawn = (EnemyType)(AGGRAVATING_ACORN + (rand() % 4));  // Tier 2-4
					else if (enemiesKilled < 25)
						typeToSpawn = (EnemyType)(MURDEROUS_MUSHROOM + (rand() % 3));  // Tier 3-4
					else
						typeToSpawn = (EnemyType)(rand() % 7);  // All types

					// Occasional boss spawn after many kills
					if (enemiesKilled > 30 && rand() % 100 < 15)
						typeToSpawn = ACORN_MAKER;

					enemies[i].initialize(spawnX, spawnY, typeToSpawn, enemyScalingMultiplier);
					break;
				}
			}
		}
	}

	void handleCollisions()
	{
		// Platform collisions
		player.setOnGround(false);
		for (int i = 0; i < platformCount; i++)
		{
			if (platforms[i].isColliding(player.getHurtbox()))
			{
				// Simple platform collision: if player is above platform, set as ground
				float platformTop = platforms[i].getTopY();
				if (player.getPosition().y + 76 > platformTop)
				{
					player.setOnGround(true);
					player.setVelocityY(0);
				}
			}
		}

		// Parry detection - player in parry state hitting pink projectiles
		if (player.isInParryState())
		{
			for (int i = 0; i < MAX_PROJECTILES; i++)
			{
				if (projectiles[i].isActive() && projectiles[i].getParryable())
				{
					if (player.getHitbox().intersects(projectiles[i].getBounds()))
					{
						// Successful parry!
						projectiles[i].setInactive();
						player.increaseMeter(15.0f);  // Increase super meter on parry
						score += 20;  // Bonus points for parry
					}
				}
			}
		}

		// Projectile-Boss collisions (only in boss level)
		if (bossLevel && !boss.isDead())
		{
			for (int i = 0; i < MAX_PROJECTILES; i++)
			{
				if (projectiles[i].isActive())
				{
					if (projectiles[i].getBounds().intersects(boss.getBounds()))
					{
						int damage = projectiles[i].getDamage();
						boss.takeDamage(damage);
						projectiles[i].setInactive();
						
						// Points for hitting boss
						int points = damage * 2;
						score += points;
						
						// Increase super meter on hit
						player.increaseMeter((float)damage / 2.0f);
					}
				}
			}
		}

		// Projectile-Enemy collisions
		for (int i = 0; i < MAX_PROJECTILES; i++)
		{
			if (projectiles[i].isActive())
			{
				for (int j = 0; j < MAX_ENEMIES; j++)
				{
					if (enemies[j].isActive())
					{
						if (projectiles[i].getBounds().intersects(enemies[j].getBounds()))
						{
							int damage = projectiles[i].getDamage();
							enemies[j].takeDamage(damage);
							projectiles[i].setInactive();
							
							// Weapon-specific point values based on damage
							int points = 0;
							switch (projectiles[i].getWeaponType())
							{
								case PEASHOOTER:
									points = 5;  // Weakest weapon, low points
									break;
								case SPREAD:
									points = 6;  // 3 projectiles but low damage
									break;
								case CHASER:
									points = 12; // Medium weapon, medium points
									break;
								case LOBBER:
									points = 18; // Strong arc weapon, high points
									break;
								case CHARGE:
									points = 25; // Strongest weapon, highest points
									break;
								case ROUNDABOUT:
									points = 14; // Strong ranged weapon
									break;
							}
							score += points;
							
							// Increase super meter on hit (scaled by damage)
							player.increaseMeter((float)damage / 2.0f);
							
							// Check if enemy died
							if (!enemies[j].isActive())
							{
								// Enemy-specific kill bonus based on enemy type
								int killBonus = 0;
								switch (enemies[j].getType())
								{
									case DEADLY_DAISY:
										killBonus = 50;
										break;
									case AGGRAVATING_ACORN:
										killBonus = 75;
										break;
									case MURDEROUS_MUSHROOM:
										killBonus = 100;
										break;
									case TERRIBLE_TULIP:
										killBonus = 60;
										break;
									case TOOTHY_TERROR:
										killBonus = 150;
										break;
									case BOTHERSOME_BLUEBERRY:
										killBonus = 40;
										break;
									case ACORN_MAKER:
										killBonus = 500;  // Mini-boss bonus
										break;
									default:
										killBonus = 50;
										break;
								}
								score += killBonus;
								enemiesKilled++;
							}
						}
					}
				}
			}
		}

		// Boss attack collision with player (only in boss level)
		if (bossLevel && !boss.isDead())
		{
			bossCollisionCooldown -= 0.016f;
			
			// Boss deals damage when attacking AND touching player
			if (boss.isCurrentlyAttacking())
			{
				FloatRect playerBox = player.getHurtbox();
				FloatRect bossBox = boss.getBounds();
				
				if (playerBox.intersects(bossBox))
				{
					if (bossCollisionCooldown <= 0)
					{
						std::cout << "BOSS HIT PLAYER! Damage applied. Lives before: " << lives << std::endl;
						// Boss attack deals damage to player - apply phase 2 scaling (20% more damage)
						int baseDamage = 20 + (rand() % 15);  // 20-35 damage per hit
						int bossDamage = (int)(baseDamage * (1.0f + (enemyScalingMultiplier - 1.0f)));  // Increase damage in phase 2
						player.takeDamage(bossDamage);
						lives--;  // Lose one life on boss collision
						bossCollisionCooldown = 0.5f;  // 0.5 second cooldown between hits
						
						if (lives > 0)
						{
							// Reset player health when losing a life
							player.resetHealth();
						}
						
						// Knockback from boss
						Vector2f playerPos = player.getPosition();
						Vector2f bossPos = boss.getPosition();
						int knockbackDir = (playerPos.x > bossPos.x) ? 1 : -1;
						player.applyKnockback(knockbackDir);
					}
				}
			}
		}

		// Enemy-Player collisions with cooldown to prevent rapid life loss
		collisionCooldown -= 0.016f;
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			if (enemies[i].isActive())
			{
				if (player.getHurtbox().intersects(enemies[i].getBounds()))
				{
					if (collisionCooldown <= 0)
					{
						lives--;  // Lose one life on collision
						collisionCooldown = 1.0f;  // 1 second cooldown between collisions
						
						if (lives > 0)
						{
							// Reset player health when losing a life
							player.resetHealth();
						}
					}
				}
			}
		}
	}

	void fireProjectile(float x, float y, float dirX, float dirY, WeaponType weapon)
	{
		shootCooldown -= 0.016f;
		if (shootCooldown > 0) return;

		int damage = 10;
		float cooldown = 0.15f;

		// Weapon-specific settings
		switch (weapon)
		{
			case PEASHOOTER:
				damage = 10;
				cooldown = 0.1f;
				break;
			case SPREAD:
				damage = 8;
				cooldown = 0.15f;
				// Fire 3 shots in spread pattern
				for (int angle = -1; angle <= 1; angle++)
				{
					float spreadX = dirX + angle * 0.3f;
					float spreadY = dirY + angle * 0.2f;
					for (int i = 0; i < MAX_PROJECTILES; i++)
					{
						if (!projectiles[i].isActive())
						{
							int scaledDamage = (int)(damage * phaseScalingMultiplier);
							projectiles[i].initialize(x, y, spreadX, spreadY, weapon, scaledDamage);
							break;
						}
					}
				}
				shootCooldown = cooldown;
				return;
			case CHASER:
				damage = 12;
				cooldown = 0.2f;
				break;
			case LOBBER:
				damage = 15;
				cooldown = 0.25f;
				break;
			case CHARGE:
				damage = 20;
				cooldown = 0.3f;
				break;
			case ROUNDABOUT:
				damage = 11;
				cooldown = 0.18f;
				break;
		}

		for (int i = 0; i < MAX_PROJECTILES; i++)
		{
			if (!projectiles[i].isActive())
			{
				int scaledDamage = (int)(damage * phaseScalingMultiplier);
				projectiles[i].initialize(x, y, dirX, dirY, weapon, scaledDamage);
				break;
			}
		}

		shootCooldown = cooldown;
	}

	void handleInput()
	{
		if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
		{
			player.moveLeft();
		}

		if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
		{
			player.moveRight();
		}

		// 360° aiming with directional input
		Vector2f playerCenter = player.getPosition() + Vector2f(36, 40);

		// Shoot Right
		if (Keyboard::isKeyPressed(Keyboard::W))
			fireProjectile(playerCenter.x, playerCenter.y, 1.0f, 0.0f, currentWeapon);

		// Shoot Left
		if (Keyboard::isKeyPressed(Keyboard::Q))
			fireProjectile(playerCenter.x, playerCenter.y, -1.0f, 0.0f, currentWeapon);

		// Shoot Up
		if (Keyboard::isKeyPressed(Keyboard::Up))
			fireProjectile(playerCenter.x, playerCenter.y, 0.0f, -1.0f, currentWeapon);

		// Shoot Down
		if (Keyboard::isKeyPressed(Keyboard::Down))
			fireProjectile(playerCenter.x, playerCenter.y, 0.0f, 1.0f, currentWeapon);

		// Diagonal shots
		if (Keyboard::isKeyPressed(Keyboard::U))  // Up-Right
			fireProjectile(playerCenter.x, playerCenter.y, 0.7f, -0.7f, currentWeapon);

		if (Keyboard::isKeyPressed(Keyboard::I))  // Up-Left
			fireProjectile(playerCenter.x, playerCenter.y, -0.7f, -0.7f, currentWeapon);

		if (Keyboard::isKeyPressed(Keyboard::O))  // Down-Right
			fireProjectile(playerCenter.x, playerCenter.y, 0.7f, 0.7f, currentWeapon);

		if (Keyboard::isKeyPressed(Keyboard::P))  // Down-Left
			fireProjectile(playerCenter.x, playerCenter.y, -0.7f, 0.7f, currentWeapon);

		// Weapon switching with number keys
		if (Keyboard::isKeyPressed(Keyboard::Num1))
			currentWeapon = PEASHOOTER;
		if (Keyboard::isKeyPressed(Keyboard::Num2))
			currentWeapon = SPREAD;
		if (Keyboard::isKeyPressed(Keyboard::Num3))
			currentWeapon = CHASER;
		if (Keyboard::isKeyPressed(Keyboard::Num4))
			currentWeapon = LOBBER;
		if (Keyboard::isKeyPressed(Keyboard::Num5))
			currentWeapon = CHARGE;
		if (Keyboard::isKeyPressed(Keyboard::Num6))
			currentWeapon = ROUNDABOUT;

		// Character switching with C key - long press required
		if (Keyboard::isKeyPressed(Keyboard::C))
		{
			cKeyPressTime += 0.016f;
			if (cKeyPressTime > 0.5f)  // 500ms long press required
			{
				player.switchCharacter();
				cKeyPressTime = 0;
			}
		}
		else
		{
			cKeyPressTime = 0;  // Reset if key is released
		}

		// Super ability activation with Enter key
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			static float superCooldown = 0;
			superCooldown += 0.016f;
			if (superCooldown > 0.3f)
			{
				player.activateSuper();
				superCooldown = 0;
			}
		}

		// Pause with ESC key
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			static float escCooldown = 0;
			escCooldown += 0.016f;
			if (escCooldown > 0.3f)
			{
				isPaused = !isPaused;
				showPauseMenu = isPaused;
				if (isPaused)
				{
					gameStateManager.pauseGame();
				}
				else
				{
					gameStateManager.resumeGame();
				}
				escCooldown = 0;
			}
		}
	}

	void draw()
	{
		// Show Phase 2 transition screen if active
		if (showingPhase2Transition)
		{
			drawPhase2TransitionScreen();
			return;
		}
		
		// Set camera view
		window.setView(camera.getView());

		window.clear(Color(135, 206, 235)); // Sky blue

		// Draw appropriate background based on level type and phase
		if (levelType == 0)  // Forest Follies
		{
			// Draw tiling forest background - repeats across the entire world
			float forestWidth = forestTexture.getSize().x;
			
			// Calculate starting position
			float cameraX = camera.getCameraX();
			float startX = (int)(cameraX / forestWidth) * forestWidth;
			
			// Draw multiple forest sprites to cover the entire world
			for (float x = startX - forestWidth; x < cameraX + SCREEN_X + forestWidth; x += forestWidth)
			{
				forestSprite.setPosition(x, 0);
				window.draw(forestSprite);
			}
		}
		else if (levelType == 1)  // Boss Level
		{
			// Choose background based on phase
			if (bossPhase == 2)
			{
				// Draw Phase 2 background tiled
				float phase2Width = phase2Texture.getSize().x;
				
				// Calculate starting position
				float cameraX = camera.getCameraX();
				float startX = (int)(cameraX / phase2Width) * phase2Width;
				
				// Draw multiple phase 2 sprites to cover the entire world
				for (float x = startX - phase2Width; x < cameraX + SCREEN_X + phase2Width; x += phase2Width)
				{
					phase2Sprite.setPosition(x, 0);
					window.draw(phase2Sprite);
				}
			}
			else
			{
				// Draw Phase 1 boss background tiled
				float bossWidth = bossTexture.getSize().x;
				
				// Calculate starting position
				float cameraX = camera.getCameraX();
				float startX = (int)(cameraX / bossWidth) * bossWidth;
				
				// Draw multiple boss sprites to cover the entire world
				for (float x = startX - bossWidth; x < cameraX + SCREEN_X + bossWidth; x += bossWidth)
				{
					bossSprite.setPosition(x, 0);
					window.draw(bossSprite);
				}
			}
		}

		// Draw ground
		Texture groundTexture;
		if (groundTexture.loadFromFile("Data/ground.png"))
		{
			Sprite groundSprite;
			groundSprite.setTexture(groundTexture);
			groundSprite.setPosition(0, GROUND_LEVEL - 2);
			groundSprite.setScale((float)WORLD_WIDTH / groundTexture.getSize().x,
				(float)(SCREEN_Y - GROUND_LEVEL) / groundTexture.getSize().y);
			window.draw(groundSprite);
		}
		else
		{
			// Fallback: Draw ground as colored rectangle
			RectangleShape groundFallback(Vector2f(WORLD_WIDTH, SCREEN_Y - GROUND_LEVEL));
			groundFallback.setPosition(0, GROUND_LEVEL);
			groundFallback.setFillColor(Color(139, 69, 19));  // Brown
			window.draw(groundFallback);
		}

		// Draw platforms
		for (int i = 0; i < platformCount; i++)
		{
			platforms[i].draw(window);
		}

		// Draw boss if in boss level
		if (bossLevel)
		{
			boss.draw(window);
		}

		// Draw goal flag
		goalFlag.draw(window);

		// Draw player
		player.draw(window);

		// Draw enemies
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			enemies[i].draw(window);
		}

		// Draw projectiles
		for (int i = 0; i < MAX_PROJECTILES; i++)
		{
			projectiles[i].draw(window);
		}

		// Reset view for UI
		window.setView(window.getDefaultView());

		// Draw score (HUD stays on screen)
		if (fontLoaded)
		{
			scoreText.setString("Score: " + to_string(score));
			window.draw(scoreText);

			// Draw weapon info
			Text weaponText;
			weaponText.setFont(font);
			weaponText.setCharacterSize(20);
			weaponText.setFillColor(Color::Yellow);
			weaponText.setPosition(10, 60);

			string weaponName = "Peashooter";
			switch (currentWeapon)
			{
				case PEASHOOTER: weaponName = "1: Peashooter"; break;
				case SPREAD: weaponName = "2: Spread"; break;
				case CHASER: weaponName = "3: Chaser"; break;
				case LOBBER: weaponName = "4: Lobber"; break;
				case CHARGE: weaponName = "5: Charge"; break;
				case ROUNDABOUT: weaponName = "6: Roundabout"; break;
			}
			weaponText.setString(weaponName);
			window.draw(weaponText);

			// Draw character name
			Text charText;
			charText.setFont(font);
			charText.setCharacterSize(16);
			charText.setFillColor(Color::White);
			charText.setPosition(10, 85);
			charText.setString("Char: " + player.getCharacterName() + " (C to switch)");
			window.draw(charText);

			// Draw super meter info
			Text superText;
			superText.setFont(font);
			superText.setCharacterSize(16);
			superText.setFillColor(Color::Magenta);
			superText.setPosition(10, 105);
			superText.setString("Super: " + to_string((int)player.getSuperMeter()) + "% (Enter to activate)");
			window.draw(superText);

			// Draw parry/double jump info
			Text parryText;
			parryText.setFont(font);
			parryText.setCharacterSize(16);
			parryText.setFillColor(Color(255, 192, 203));  // Pink
			parryText.setPosition(10, 125);
			parryText.setString("Space: Jump, Double Space: Parry (hit pink projectiles!)");
			window.draw(parryText);

			// Draw lives counter
			Text livesText;
			livesText.setFont(font);
			livesText.setCharacterSize(20);
			livesText.setFillColor(Color::Red);
			livesText.setPosition(SCREEN_X - 150, 10);
			livesText.setString("Lives: " + to_string(lives));
			window.draw(livesText);
			
			// Draw pause and rewind info
			Text pauseInfoText;
			pauseInfoText.setFont(font);
			pauseInfoText.setCharacterSize(14);
			pauseInfoText.setFillColor(Color::Cyan);
			pauseInfoText.setPosition(10, 145);
			float rewindTimeRemaining = maxRewindTime - rewindTimer;
			if (rewindTimeRemaining < 0) rewindTimeRemaining = 0;
			pauseInfoText.setString("Rewind Available: " + to_string((int)rewindTimeRemaining) + "s (Press ESC to pause)");
			window.draw(pauseInfoText);
		}

		// Draw pause menu if paused
		if (isPaused && showPauseMenu)
		{
			drawPauseMenu();
		}

		window.display();
	}

	void drawPauseMenu()
	{
		// Darken the background
		RectangleShape overlay(Vector2f(SCREEN_X, SCREEN_Y));
		overlay.setFillColor(Color(0, 0, 0, 150));
		window.draw(overlay);

		// Pause title
		Text pauseTitle;
		pauseTitle.setFont(font);
		pauseTitle.setString("GAME PAUSED");
		pauseTitle.setCharacterSize(60);
		pauseTitle.setFillColor(Color::Yellow);
		FloatRect textBounds = pauseTitle.getLocalBounds();
		pauseTitle.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 - 150);
		window.draw(pauseTitle);

		// Rewind button
		RectangleShape rewindButton(Vector2f(300, 60));
		rewindButton.setPosition(SCREEN_X / 2 - 150, SCREEN_Y / 2);
		rewindButton.setFillColor(Color::Blue);
		rewindButton.setOutlineThickness(3);
		rewindButton.setOutlineColor(Color::White);
		window.draw(rewindButton);

		Text rewindText;
		rewindText.setFont(font);
		rewindText.setString("R - Rewind 10 Seconds");
		rewindText.setCharacterSize(24);
		rewindText.setFillColor(Color::White);
		textBounds = rewindText.getLocalBounds();
		rewindText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 + 15);
		window.draw(rewindText);

		// Exit button
		RectangleShape exitButton(Vector2f(300, 60));
		exitButton.setPosition(SCREEN_X / 2 - 150, SCREEN_Y / 2 + 100);
		exitButton.setFillColor(Color::Red);
		exitButton.setOutlineThickness(3);
		exitButton.setOutlineColor(Color::White);
		window.draw(exitButton);

		Text exitText;
		exitText.setFont(font);
		exitText.setString("E - Exit to Menu");
		exitText.setCharacterSize(24);
		exitText.setFillColor(Color::White);
		textBounds = exitText.getLocalBounds();
		exitText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 + 115);
		window.draw(exitText);

		// Save Game button
		RectangleShape saveButton(Vector2f(300, 60));
		saveButton.setPosition(SCREEN_X / 2 - 150, SCREEN_Y / 2 + 200);
		saveButton.setFillColor(Color::Green);
		saveButton.setOutlineThickness(3);
		saveButton.setOutlineColor(Color::White);
		window.draw(saveButton);

		Text saveText;
		saveText.setFont(font);
		saveText.setString("S - Save Game");
		saveText.setCharacterSize(24);
		saveText.setFillColor(Color::White);
		textBounds = saveText.getLocalBounds();
		saveText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 + 215);
		window.draw(saveText);

		// Save Slot Info
		Text slotText;
		slotText.setFont(font);
		slotText.setString("Slot: " + std::to_string(currentSaveSlot + 1) + "  (Q/W to Change)");
		slotText.setCharacterSize(18);
		slotText.setFillColor(Color::Magenta);
		textBounds = slotText.getLocalBounds();
		slotText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 + 270);
		window.draw(slotText);

		// Resume info
		Text resumeText;
		resumeText.setFont(font);
		resumeText.setString("Press ESC to Resume");
		resumeText.setCharacterSize(20);
		resumeText.setFillColor(Color::Cyan);
		textBounds = resumeText.getLocalBounds();
		resumeText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 + 300);
		window.draw(resumeText);
	}

	int getScore() const { return score; }
	int getPlayerHealth() const { return player.getHealth(); }
	bool isGameOver() const { return lives <= 0; }
	bool isLevelComplete() const { return levelComplete; }
	bool shouldExit() const { return shouldExitToMenu; }
	void setExitToMenu() { shouldExitToMenu = true; }
	
	void stop() 
	{ 
		// Audio disabled - no cleanup needed
	}
	
	void drawGameOverScreen()
	{
		// Reset view to default for UI
		window.setView(window.getDefaultView());
		window.clear(Color(0, 0, 0));

		// Game Over text
		Text gameOverText;
		gameOverText.setFont(font);
		gameOverText.setString("GAME OVER");
		gameOverText.setCharacterSize(80);
		gameOverText.setFillColor(Color::Red);
		FloatRect textBounds = gameOverText.getLocalBounds();
		gameOverText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 - 150);
		window.draw(gameOverText);

		// Score text
		Text finalScoreText;
		finalScoreText.setFont(font);
		finalScoreText.setString("Final Score: " + to_string(score));
		finalScoreText.setCharacterSize(50);
		finalScoreText.setFillColor(Color::White);
		textBounds = finalScoreText.getLocalBounds();
		finalScoreText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2);
		window.draw(finalScoreText);

		// Press key to exit text
		Text exitText;
		exitText.setFont(font);
		exitText.setString("Press any key to exit");
		exitText.setCharacterSize(30);
		exitText.setFillColor(Color::Yellow);
		textBounds = exitText.getLocalBounds();
		exitText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 + 100);
		window.draw(exitText);

		window.display();
	}

	void drawLevelCompleteScreen()
	{
		// Reset view to default for UI
		window.setView(window.getDefaultView());
		window.clear(Color(0, 0, 0));

		// Determine completion message based on phase
		std::string completeMessage = "LEVEL COMPLETE!";
		std::string scoreMessage = "Score: " + to_string(score);
		
		if (bossLevel && bossPhase == 2)
		{
			completeMessage = "BOTH PHASES DEFEATED!";
			scoreMessage = "Final Score: " + to_string(score);
		}

		// Level Complete text
		Text completeText;
		completeText.setFont(font);
		completeText.setString(completeMessage);
		completeText.setCharacterSize(80);
		completeText.setFillColor(Color::Green);
		FloatRect textBounds = completeText.getLocalBounds();
		completeText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 - 150);
		window.draw(completeText);

		// Score text
		Text scoreDisplayText;
		scoreDisplayText.setFont(font);
		scoreDisplayText.setString(scoreMessage);
		scoreDisplayText.setCharacterSize(50);
		scoreDisplayText.setFillColor(Color::White);
		textBounds = scoreDisplayText.getLocalBounds();
		scoreDisplayText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2);
		window.draw(scoreDisplayText);

		// Press key to continue text
		Text continueText;
		continueText.setFont(font);
		continueText.setString("Press any key to continue");
		continueText.setCharacterSize(30);
		continueText.setFillColor(Color::Yellow);
		textBounds = continueText.getLocalBounds();
		continueText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 + 100);
		window.draw(continueText);

		window.display();
	}

	bool isPauseMenuOpen() const { return showPauseMenu; }

	void playerJump()
	{
		if (!isPaused)  // Only allow jump if game is not paused
		{
			player.jump();
		}
	}

	void drawPhase2TransitionScreen()
	{
		// Reset view to default for UI
		window.setView(window.getDefaultView());
		window.clear(Color(0, 0, 0));

		// Phase 2 title
		Text phase2Text;
		phase2Text.setFont(font);
		phase2Text.setString("PHASE 2");
		phase2Text.setCharacterSize(100);
		phase2Text.setFillColor(Color::Red);
		FloatRect textBounds = phase2Text.getLocalBounds();
		phase2Text.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 - 150);
		window.draw(phase2Text);

		// Score and lives display
		Text scoreText;
		scoreText.setFont(font);
		scoreText.setString("Score: " + to_string(score) + "  Lives: " + to_string(lives));
		scoreText.setCharacterSize(40);
		scoreText.setFillColor(Color::White);
		textBounds = scoreText.getLocalBounds();
		scoreText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 - 10);
		window.draw(scoreText);

		// Difficulty message
		Text diffText;
		diffText.setFont(font);
		diffText.setString("Increased Difficulty!");
		diffText.setCharacterSize(40);
		diffText.setFillColor(Color::Yellow);
		textBounds = diffText.getLocalBounds();
		diffText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 + 80);
		window.draw(diffText);

		// Starting message
		Text startText;
		startText.setFont(font);
		startText.setString("Get ready...");
		startText.setCharacterSize(30);
		startText.setFillColor(Color::Cyan);
		textBounds = startText.getLocalBounds();
		startText.setPosition(SCREEN_X / 2 - textBounds.width / 2, SCREEN_Y / 2 + 160);
		window.draw(startText);

		window.display();
	}

	void rewind()
	{
		// Calculate how many frames ago we want to go back
		int framesBack = 0;
		
		if (rewindTimer < maxRewindTime)
		{
			// Rewind to start of level
			framesBack = (int)(rewindTimer / 0.01f);  // snapshots every 0.01s
		}
		else
		{
			// Rewind exactly 10 seconds
			framesBack = (int)(maxRewindTime / 0.01f);
		}
		
		std::cout << "Rewind: Going back " << framesBack << " frames (approx " << (framesBack * 0.01f) << " seconds)" << std::endl;
		
		// Get the snapshot
		if (gameStateManager.getBufferSize() > 0)
		{
			GameStateSnapshot snapshot = gameStateManager.getSnapshotAtOffset(framesBack);
			restoreGameState(snapshot);
			
			std::cout << "Game state restored!" << std::endl;
		}
		else
		{
			std::cout << "Snapshot buffer empty!" << std::endl;
		}
		
		// Resume game
		isPaused = false;
		showPauseMenu = false;
		gameStateManager.resumeGame();
		rewindTimer = 0;
	}

	void recordGameState()
	{
		GameStateSnapshot snapshot;
		
		// Record player state
		snapshot.playerState.position = player.getPosition();
		snapshot.playerState.velocityY = player.getVelocityY();
		snapshot.playerState.onGround = player.isOnGround();
		snapshot.playerState.currentHealth = player.getHealth();
		snapshot.playerState.superMeter = player.getSuperMeter();
		snapshot.playerState.currentCharType = player.getCharacterType();
		snapshot.playerState.currentWeapon = currentWeapon;
		
		// Record enemy states
		snapshot.enemyCount = 0;
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			if (enemies[i].isActive())
			{
				EnemyStateSnapshot enemySnap;
				enemySnap.position = enemies[i].getPosition();
				enemySnap.active = true;
				enemySnap.health = enemies[i].getHealth();
				enemySnap.maxHealth = enemies[i].getMaxHealth();
				enemySnap.enemyType = enemies[i].getType();
				snapshot.enemyStates.push_back(enemySnap);
				snapshot.enemyCount++;
			}
		}
		
		// Record projectile states
		snapshot.projectileCount = 0;
		for (int i = 0; i < MAX_PROJECTILES; i++)
		{
			if (projectiles[i].isActive())
			{
				ProjectileStateSnapshot projSnap;
				projSnap.position = projectiles[i].getPosition();
				projSnap.active = true;
				projSnap.weaponType = projectiles[i].getWeaponType();
				projSnap.damage = projectiles[i].getDamage();
				snapshot.projectileStates.push_back(projSnap);
				snapshot.projectileCount++;
			}
		}
		
		snapshot.score = score;
		snapshot.lives = lives;
		snapshot.elapsedTime = levelElapsedTime;
		snapshot.platformCount = platformCount;
		
		gameStateManager.recordSnapshot(snapshot);
	}

	void restoreGameState(const GameStateSnapshot& snapshot)
	{
		// Restore player
		player.setPosition(snapshot.playerState.position);
		player.setVelocityY(snapshot.playerState.velocityY);
		player.setOnGround(snapshot.playerState.onGround);
		
		// Restore player character and health
		player.setState(
			snapshot.playerState.position,
			snapshot.playerState.velocityY,
			snapshot.playerState.onGround,
			snapshot.playerState.currentCharType,
			snapshot.playerState.currentHealth,
			snapshot.playerState.currentWeapon
		);
		
		// Restore current weapon
		currentWeapon = (WeaponType)snapshot.playerState.currentWeapon;
		
		// Clear all active enemies and projectiles
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			// Mark as inactive
			enemies[i] = Enemy();
		}
		for (int i = 0; i < MAX_PROJECTILES; i++)
		{
			projectiles[i] = Projectile();
		}
		
		// Restore enemies
		for (int i = 0; i < (int)snapshot.enemyStates.size() && i < MAX_ENEMIES; i++)
		{
			const EnemyStateSnapshot& esnap = snapshot.enemyStates[i];
			enemies[i].initialize(esnap.position.x, esnap.position.y, (EnemyType)esnap.enemyType);
			// Set health
			while (enemies[i].getHealth() > esnap.health)
			{
				enemies[i].takeDamage(1);
			}
		}
		
		// Restore projectiles
		for (int i = 0; i < (int)snapshot.projectileStates.size() && i < MAX_PROJECTILES; i++)
		{
			const ProjectileStateSnapshot& psnap = snapshot.projectileStates[i];
			projectiles[i].initialize(psnap.position.x, psnap.position.y, 0, 0, (WeaponType)psnap.weaponType, psnap.damage);
		}
		
		// Restore game data
		score = snapshot.score;
		lives = snapshot.lives;
		levelElapsedTime = snapshot.elapsedTime;
		rewindTimer = snapshot.elapsedTime;
		
		std::cout << "Game state fully restored!" << std::endl;
	}

	void saveGame(int levelType)
	{
		// Determine level name
		std::string levelName = (levelType == 0) ? "Forest Follies" : "Boss Level";
		
		// Record current game state
		recordGameState();
		
		// Get latest snapshot (most recent, at offset 0)
		GameStateSnapshot snapshot = gameStateManager.getSnapshotAtOffset(0);
		
		// Save to current selected slot
		saveGameManager.saveGameToSlot(currentSaveSlot, snapshot, levelName, score, lives, levelElapsedTime);
		
		std::cout << "Game saved successfully to slot " << (currentSaveSlot + 1) << "!" << std::endl;
		
		// Auto-return to menu after save
		setExitToMenu();
	}

	void loadGame(int slotIndex)
	{
		GameStateSnapshot snapshot;
		std::string levelName;
		int savedScore, savedLives;
		float savedPlaytime;
		
		// Load from slot
		if (saveGameManager.loadGameFromSlot(slotIndex, snapshot, levelName, savedScore, savedLives, savedPlaytime))
		{
			std::cout << "Game loaded successfully from slot " << slotIndex << std::endl;
			// Restore game state
			restoreGameState(snapshot);
		}
		else
		{
			std::cout << "Failed to load game from slot " << slotIndex << std::endl;
		}
	}

	void changeSaveSlot(int direction)
	{
		currentSaveSlot += direction;
		
		// Wrap around (0-4)
		if (currentSaveSlot < 0)
			currentSaveSlot = 4;
		else if (currentSaveSlot > 4)
			currentSaveSlot = 0;
		
		std::cout << "Save slot changed to: " << (currentSaveSlot + 1) << std::endl;
	}
};

int main()
{
	RenderWindow window(VideoMode(SCREEN_X, SCREEN_Y), "Cuphead - Complete", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	Menu menu;
	Event ev;
	bool showMenu = true;
	int selectedLevel = 0;
	int loadGameSlot = -1;  // -1 means don't load, otherwise load from this slot

	// Main loop
	while (window.isOpen())
	{
		// Menu screen
		while (showMenu && window.isOpen())
		{
			while (window.pollEvent(ev))
			{
				if (ev.type == Event::Closed)
				{
					window.close();
				}
				
				// Handle mouse clicks on menu
				if (ev.type == Event::MouseButtonPressed)
				{
					if (ev.mouseButton.button == sf::Mouse::Left)
					{
						sf::Vector2f mousePos(ev.mouseButton.x, ev.mouseButton.y);
						Menu::MenuState state = menu.handleMouseClick(mousePos);
						
						if (state == Menu::START_GAME)
						{
							selectedLevel = (menu.getSelectedLevel() == Menu::FOREST_FOLLIES) ? 0 : 1;
							loadGameSlot = -1;  // Don't load, start fresh
							showMenu = false;
							break;
						}
						else if (state == Menu::LOAD_GAME)
						{
							// Load game from selected slot
							loadGameSlot = menu.getSelectedLoadSlot();
							showMenu = false;  // Exit menu and start loading
							break;
						}
						else if (state == Menu::EXIT_GAME)
						{
							window.close();
							return 0;
						}
					}
				}
				
				// Handle ESC key on scores screen
				if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape)
				{
					if (menu.getCurrentState() == Menu::SHOWING_SCORES)
					{
						menu.resetToMenu();
					}
					else if (menu.getCurrentState() == Menu::SHOWING_LEVEL_SELECT)
					{
						menu.resetToMenu();
					}
					else if (menu.getCurrentState() == Menu::SHOWING_LOAD_GAME)
					{
						menu.resetToMenu();
					}
				}
			}

			menu.update();
			menu.draw(window);
		}

		if (!window.isOpen())
			break;

		// Create game manager with selected level
		GameManager gameManager(window, selectedLevel);
		
		// Load game if needed
		if (loadGameSlot >= 0)
		{
			gameManager.loadGame(loadGameSlot);
			loadGameSlot = -1;  // Reset for next iteration
		}

		// Game loop
		while (window.isOpen())
		{
			while (window.pollEvent(ev))
			{
				if (ev.type == Event::Closed)
				{
					window.close();
				}

				if (ev.type == Event::KeyPressed)
				{
					// Handle jump separately (needs single press, not continuous)
					if (ev.key.code == Keyboard::Space && !gameManager.isPauseMenuOpen())
					{
						gameManager.playerJump();
					}
					
					// Handle pause menu inputs
					if (gameManager.isPauseMenuOpen())
					{
						if (ev.key.code == Keyboard::R)
						{
							// Rewind 10 seconds
							gameManager.rewind();
						}
						else if (ev.key.code == Keyboard::E)
						{
							// Exit to menu
							gameManager.setExitToMenu();
						}
						else if (ev.key.code == Keyboard::S)
						{
							// Save game
							gameManager.saveGame(selectedLevel);
						}
						else if (ev.key.code == Keyboard::Q)
						{
							// Previous save slot
							gameManager.changeSaveSlot(-1);
						}
						else if (ev.key.code == Keyboard::W)
						{
							// Next save slot
							gameManager.changeSaveSlot(1);
						}
					}
				}
			}

			gameManager.handleInput();
			gameManager.update();
			gameManager.draw();

			// Check if player wants to exit to menu from pause
			if (gameManager.shouldExit())
			{
				gameManager.stop();
				menu.resetToMenu();
				showMenu = true;
				break;
			}

			if (gameManager.isLevelComplete())
			{
				gameManager.stop();
				
				// Add score to leaderboard
				menu.addScore(gameManager.getScore());
				
				// Display level complete screen
				bool waitingForKeyPress = true;
				while (window.isOpen() && waitingForKeyPress)
				{
					while (window.pollEvent(ev))
					{
						if (ev.type == Event::Closed)
						{
							window.close();
							waitingForKeyPress = false;
						}
						if (ev.type == Event::KeyPressed)
						{
							waitingForKeyPress = false;
						}
					}

					gameManager.drawLevelCompleteScreen();
				}
				
				// Return to menu
				menu.resetToMenu();
				showMenu = true;
				break;
			}

			if (gameManager.isGameOver())
			{
				gameManager.stop();
				
				// Add final score to leaderboard
				menu.addScore(gameManager.getScore());
				menu.resetToMenu();
				
				// Display game over screen
				bool waitingForKeyPress = true;
				while (window.isOpen() && waitingForKeyPress)
				{
					while (window.pollEvent(ev))
					{
						if (ev.type == Event::Closed)
						{
							window.close();
							waitingForKeyPress = false;
						}
						if (ev.type == Event::KeyPressed)
						{
							waitingForKeyPress = false;
						}
					}

					gameManager.drawGameOverScreen();
				}
				
				// Return to menu
				showMenu = true;
				break;
			}
		}
	}

	return 0;
}
