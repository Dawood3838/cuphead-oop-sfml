#include "gamestate.h"
#include <iostream>

GameStateManager::GameStateManager() : snapshotTimer(0), isGamePaused(false)
{
	SNAPSHOT_INTERVAL = 0.01f;
}

GameStateManager::~GameStateManager()
{
	clearBuffer();
}

void GameStateManager::recordSnapshot(const GameStateSnapshot& snapshot)
{
	if (snapshotBuffer.size() >= MAX_SNAPSHOTS)
	{
		snapshotBuffer.pop_front();  // Remove oldest snapshot
	}
	snapshotBuffer.push_back(snapshot);
}

void GameStateManager::clearBuffer()
{
	snapshotBuffer.clear();
}

GameStateSnapshot GameStateManager::getSnapshotAtOffset(int frameOffset)
{
	if (snapshotBuffer.empty())
	{
		return GameStateSnapshot();
	}
	
	// Calculate index from the end
	int index = (int)snapshotBuffer.size() - 1 - frameOffset;
	if (index < 0) index = 0;
	
	return snapshotBuffer[index];
}

void GameStateManager::updateTimer(float deltaTime)
{
	if (!isGamePaused)
	{
		snapshotTimer += deltaTime;
	}
}

