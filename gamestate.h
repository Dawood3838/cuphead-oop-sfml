#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>
#include "constants.h"

using namespace sf;

// ==================== GAME STATE SNAPSHOT ====================
struct PlayerStateSnapshot
{
	Vector2f position;
	float velocityY;
	bool onGround;
	bool canDoubleJump;
	int currentHealth;
	float superMeter;
	int currentCharType;
	int currentWeapon;
	float speed;
	bool isMoving;
	bool takingDamage;
	int facingDirection;
};

struct EnemyStateSnapshot
{
	Vector2f position;
	float velocityX;
	float velocityY;
	int health;
	int maxHealth;
	bool active;
	int enemyType;
	float size;
};

struct ProjectileStateSnapshot
{
	Vector2f position;
	Vector2f velocity;
	bool active;
	int weaponType;
	int damage;
};

struct GameStateSnapshot
{
	PlayerStateSnapshot playerState;
	std::vector<EnemyStateSnapshot> enemyStates;
	std::vector<ProjectileStateSnapshot> projectileStates;
	float elapsedTime;
	int score;
	int lives;
	int platformCount;
	int enemyCount;
	int projectileCount;
};

// ==================== GAME STATE MANAGER ====================
class GameStateManager
{
private:
	std::deque<GameStateSnapshot> snapshotBuffer;
	static const int MAX_SNAPSHOTS = 1000;  // ~10 seconds at 100 FPS
	float SNAPSHOT_INTERVAL;  // Take snapshot every 10ms
	float snapshotTimer;
	bool isGamePaused;

public:
	GameStateManager();
	~GameStateManager();

	// Snapshot management
	void recordSnapshot(const GameStateSnapshot& snapshot);
	void clearBuffer();
	GameStateSnapshot getSnapshotAtOffset(int frameOffset);
	int getBufferSize() const { return snapshotBuffer.size(); }

	// Pause management
	void pauseGame() { isGamePaused = true; }
	void resumeGame() { isGamePaused = false; }
	bool isPaused() const { return isGamePaused; }

	// Timer management
	void updateTimer(float deltaTime);
	bool shouldSnapshot() { return snapshotTimer >= SNAPSHOT_INTERVAL; }
	void resetTimer() { snapshotTimer = 0; }
};

#endif // GAMESTATE_H
