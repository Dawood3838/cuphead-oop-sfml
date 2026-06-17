#include "savegame.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

const std::string SaveGameManager::SAVE_DIR = "Data/Saves/";

SaveGameManager::SaveGameManager()
{
	// Create save directory if it doesn't exist
	mkdir(SAVE_DIR.c_str(), 0755);
	
	// Initialize save slots
	for (int i = 0; i < MAX_SAVE_SLOTS; i++)
	{
		saveSlots[i].filename = getSaveFilepath(i);
		saveSlots[i].isUsed = false;
		saveSlots[i].score = 0;
		saveSlots[i].lives = 0;
		saveSlots[i].playtime = 0;
	}
	
	// Load save slot info
	loadSaveSlotInfo();
}

SaveGameManager::~SaveGameManager()
{
	// Cleanup
}

std::string SaveGameManager::getCurrentTimestamp()
{
	time_t now = time(nullptr);
	struct tm* timeinfo = localtime(&now);
	
	std::stringstream ss;
	ss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

std::string SaveGameManager::getSaveFilepath(int slotIndex)
{
	std::stringstream ss;
	ss << SAVE_DIR << "save_slot_" << slotIndex << ".dat";
	return ss.str();
}

void SaveGameManager::loadSaveSlotInfo()
{
	for (int i = 0; i < MAX_SAVE_SLOTS; i++)
	{
		// Reset slot info
		saveSlots[i].isUsed = false;
		saveSlots[i].levelName = "";
		saveSlots[i].score = 0;
		saveSlots[i].lives = 0;
		saveSlots[i].playtime = 0;
		saveSlots[i].timestamp = "";
		
		std::string filepath = getSaveFilepath(i);
		std::ifstream file(filepath, std::ios::binary);
		
		if (file.is_open() && file.good())
		{
			// Read header info
			int levelLen = 0;
			file.read((char*)&levelLen, sizeof(int));
			
			if (levelLen > 0 && levelLen < 256 && file.good())
			{
				char levelBuffer[256] = {0};
				file.read(levelBuffer, levelLen);
				if (file.good())
				{
					saveSlots[i].levelName = std::string(levelBuffer, levelLen);
					
					file.read((char*)&saveSlots[i].score, sizeof(int));
					file.read((char*)&saveSlots[i].lives, sizeof(int));
					file.read((char*)&saveSlots[i].playtime, sizeof(float));
					
					if (file.good())
					{
						int timestampLen = 0;
						file.read((char*)&timestampLen, sizeof(int));
						if (timestampLen > 0 && timestampLen < 256 && file.good())
						{
							char timestampBuffer[256] = {0};
							file.read(timestampBuffer, timestampLen);
							if (file.good())
							{
								saveSlots[i].timestamp = std::string(timestampBuffer, timestampLen);
								saveSlots[i].isUsed = true;
								saveSlots[i].filename = filepath;
							}
						}
					}
				}
			}
			
			file.close();
		}
	}
}

bool SaveGameManager::saveGameToSlot(int slotIndex, const GameStateSnapshot& gameState,
                                     const std::string& levelName, int score, int lives, float playtime)
{
	if (slotIndex < 0 || slotIndex >= MAX_SAVE_SLOTS)
	{
		std::cerr << "Invalid save slot index: " << slotIndex << std::endl;
		return false;
	}
	
	std::string filepath = getSaveFilepath(slotIndex);
	std::ofstream file(filepath, std::ios::binary);
	
	if (!file.is_open())
	{
		std::cerr << "Failed to open save file: " << filepath << std::endl;
		return false;
	}
	
	try
	{
		// Write header info
		int levelLen = levelName.length();
		file.write((char*)&levelLen, sizeof(int));
		file.write(levelName.c_str(), levelLen);
		
		file.write((char*)&score, sizeof(int));
		file.write((char*)&lives, sizeof(int));
		file.write((char*)&playtime, sizeof(float));
		
		std::string timestamp = getCurrentTimestamp();
		int timestampLen = timestamp.length();
		file.write((char*)&timestampLen, sizeof(int));
		file.write(timestamp.c_str(), timestampLen);
		
		// Write game state snapshot
		file.write((char*)&gameState.playerState, sizeof(PlayerStateSnapshot));
		
		// Write enemy states
		int enemyCount = gameState.enemyStates.size();
		file.write((char*)&enemyCount, sizeof(int));
		for (const auto& enemy : gameState.enemyStates)
		{
			file.write((char*)&enemy, sizeof(EnemyStateSnapshot));
		}
		
		// Write projectile states
		int projCount = gameState.projectileStates.size();
		file.write((char*)&projCount, sizeof(int));
		for (const auto& proj : gameState.projectileStates)
		{
			file.write((char*)&proj, sizeof(ProjectileStateSnapshot));
		}
		
		// Write other game data
		file.write((char*)&gameState.score, sizeof(int));
		file.write((char*)&gameState.lives, sizeof(int));
		file.write((char*)&gameState.elapsedTime, sizeof(float));
		file.write((char*)&gameState.platformCount, sizeof(int));
		file.write((char*)&gameState.enemyCount, sizeof(int));
		file.write((char*)&gameState.projectileCount, sizeof(int));
		
		file.close();
		
		// Update slot info
		saveSlots[slotIndex].levelName = levelName;
		saveSlots[slotIndex].score = score;
		saveSlots[slotIndex].lives = lives;
		saveSlots[slotIndex].playtime = playtime;
		saveSlots[slotIndex].timestamp = timestamp;
		saveSlots[slotIndex].isUsed = true;
		saveSlots[slotIndex].filename = filepath;
		
		std::cout << "Game saved to slot " << slotIndex << std::endl;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error saving game: " << e.what() << std::endl;
		file.close();
		return false;
	}
}

bool SaveGameManager::loadGameFromSlot(int slotIndex, GameStateSnapshot& gameState,
                                       std::string& levelName, int& score, int& lives, float& playtime)
{
	if (slotIndex < 0 || slotIndex >= MAX_SAVE_SLOTS || !saveSlots[slotIndex].isUsed)
	{
		std::cerr << "Save slot " << slotIndex << " is not used." << std::endl;
		return false;
	}
	
	std::ifstream file(saveSlots[slotIndex].filename, std::ios::binary);
	
	if (!file.is_open())
	{
		std::cerr << "Failed to open save file." << std::endl;
		return false;
	}
	
	try
	{
		// Read header info
		int levelLen = 0;
		file.read((char*)&levelLen, sizeof(int));
		if (levelLen > 0 && levelLen < 256)
		{
			char levelBuffer[256];
			file.read(levelBuffer, levelLen);
			levelName = std::string(levelBuffer, levelLen);
		}
		
		file.read((char*)&score, sizeof(int));
		file.read((char*)&lives, sizeof(int));
		file.read((char*)&playtime, sizeof(float));
		
		int timestampLen = 0;
		file.read((char*)&timestampLen, sizeof(int));
		if (timestampLen > 0 && timestampLen < 256)
		{
			char timestampBuffer[256];
			file.read(timestampBuffer, timestampLen);
			// timestamp not needed for loading, just skip
		}
		
		// Read game state snapshot
		file.read((char*)&gameState.playerState, sizeof(PlayerStateSnapshot));
		
		// Read enemy states
		int enemyCount = 0;
		file.read((char*)&enemyCount, sizeof(int));
		gameState.enemyStates.resize(enemyCount);
		for (int i = 0; i < enemyCount; i++)
		{
			file.read((char*)&gameState.enemyStates[i], sizeof(EnemyStateSnapshot));
		}
		
		// Read projectile states
		int projCount = 0;
		file.read((char*)&projCount, sizeof(int));
		gameState.projectileStates.resize(projCount);
		for (int i = 0; i < projCount; i++)
		{
			file.read((char*)&gameState.projectileStates[i], sizeof(ProjectileStateSnapshot));
		}
		
		// Read other game data
		file.read((char*)&gameState.score, sizeof(int));
		file.read((char*)&gameState.lives, sizeof(int));
		file.read((char*)&gameState.elapsedTime, sizeof(float));
		file.read((char*)&gameState.platformCount, sizeof(int));
		file.read((char*)&gameState.enemyCount, sizeof(int));
		file.read((char*)&gameState.projectileCount, sizeof(int));
		
		file.close();
		
		std::cout << "Game loaded from slot " << slotIndex << std::endl;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error loading game: " << e.what() << std::endl;
		file.close();
		return false;
	}
}

bool SaveGameManager::deleteSaveSlot(int slotIndex)
{
	if (slotIndex < 0 || slotIndex >= MAX_SAVE_SLOTS)
	{
		return false;
	}
	
	if (std::remove(saveSlots[slotIndex].filename.c_str()) == 0)
	{
		saveSlots[slotIndex].isUsed = false;
		saveSlots[slotIndex].score = 0;
		saveSlots[slotIndex].lives = 0;
		saveSlots[slotIndex].playtime = 0;
		saveSlots[slotIndex].levelName = "";
		saveSlots[slotIndex].timestamp = "";
		
		std::cout << "Save slot " << slotIndex << " deleted." << std::endl;
		return true;
	}
	
	return false;
}

SaveGameSlot SaveGameManager::getSaveSlot(int slotIndex) const
{
	if (slotIndex >= 0 && slotIndex < MAX_SAVE_SLOTS)
	{
		return saveSlots[slotIndex];
	}
	return SaveGameSlot();
}

int SaveGameManager::getUsedSlotsCount() const
{
	int count = 0;
	for (int i = 0; i < MAX_SAVE_SLOTS; i++)
	{
		if (saveSlots[i].isUsed)
			count++;
	}
	return count;
}

bool SaveGameManager::isSlotUsed(int slotIndex) const
{
	if (slotIndex >= 0 && slotIndex < MAX_SAVE_SLOTS)
	{
		return saveSlots[slotIndex].isUsed;
	}
	return false;
}
