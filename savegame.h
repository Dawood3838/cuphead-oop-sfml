#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <fstream>
#include "gamestate.h"

using namespace sf;

// ==================== SAVE GAME SLOT ====================
struct SaveGameSlot
{
	std::string filename;
	std::string levelName;
	int score;
	int lives;
	float playtime;
	bool isUsed;
	std::string timestamp;  // When the save was created
};

// ==================== SAVE GAME MANAGER ====================
class SaveGameManager
{
private:
	static const int MAX_SAVE_SLOTS = 5;
	static const std::string SAVE_DIR;
	
	SaveGameSlot saveSlots[MAX_SAVE_SLOTS];
	
	std::string getCurrentTimestamp();
	std::string getSaveFilepath(int slotIndex);
	
public:
	SaveGameManager();
	~SaveGameManager();
	
	// Load save slot information
	void loadSaveSlotInfo();
	
	// Save game to slot
	bool saveGameToSlot(int slotIndex, const GameStateSnapshot& gameState, 
	                     const std::string& levelName, int score, int lives, float playtime);
	
	// Load game from slot
	bool loadGameFromSlot(int slotIndex, GameStateSnapshot& gameState,
	                       std::string& levelName, int& score, int& lives, float& playtime);
	
	// Delete save from slot
	bool deleteSaveSlot(int slotIndex);
	
	// Get save slot info
	SaveGameSlot getSaveSlot(int slotIndex) const;
	int getUsedSlotsCount() const;
	
	// Check if slot is used
	bool isSlotUsed(int slotIndex) const;
};

#endif // SAVEGAME_H
