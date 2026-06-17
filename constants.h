#ifndef CONSTANTS_H
#define CONSTANTS_H

// Screen dimensions
const int SCREEN_X = 1136;
const int SCREEN_Y = 896;

// Physics constants
const float GRAVITY = 1.0f;
const float TERMINAL_VELOCITY = 20.0f;
const float JUMP_STRENGTH = -20.0f;
const float PLAYER_SPEED = 5.0f;
const float GROUND_LEVEL = SCREEN_Y - 100;

// Game limits
const int MAX_PROJECTILES = 50;
const int MAX_ENEMIES = 3;
const int MAX_PLATFORMS = 20;
const int MAX_SPAWN_POINTS = 10;
const float WORLD_WIDTH = 5000.0f;

// Weapon types
enum WeaponType
{
	PEASHOOTER,
	SPREAD,
	CHASER,
	LOBBER,
	CHARGE,
	ROUNDABOUT
};

// Enemy types (7 distinct enemies + 1 mini-boss)
enum EnemyType
{
	DEADLY_DAISY,         // Tier 1: Weak, slow
	AGGRAVATING_ACORN,    // Tier 2: Medium, ranged
	MURDEROUS_MUSHROOM,   // Tier 3: Tanky
	TERRIBLE_TULIP,       // Tier 3: Fast, evasive
	TOOTHY_TERROR,        // Tier 4: Strong, aggressive
	BOTHERSOME_BLUEBERRY, // Tier 4: Quick, deadly
	ACORN_MAKER           // Mini-boss: High health, complex AI
};

// Character types
enum CharacterType
{
	CUPHEAD,
	MUGMAN
};

#endif // CONSTANTS_H
