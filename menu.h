#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "savegame.h"

class Menu
{
public:
    enum MenuState
    {
        SHOWING_MENU,
        SHOWING_SCORES,
        SHOWING_LEVEL_SELECT,
        SHOWING_LOAD_GAME,
        START_GAME,
        LOAD_GAME,
        EXIT_GAME
    };

    enum LevelType
    {
        FOREST_FOLLIES,
        BOSS_LEVEL
    };

    Menu();
    ~Menu();

    MenuState handleMouseClick(sf::Vector2f mousePos);
    void update();
    void draw(sf::RenderWindow& window);
    void addScore(int score);
    void loadScores();
    void saveScores();
    MenuState getCurrentState() const { return currentState; }
    void resetToMenu() { currentState = SHOWING_MENU; }
    LevelType getSelectedLevel() const { return selectedLevel; }
    int getSelectedLoadSlot() const { return selectedLoadSlot; }

private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture scoreboardTexture;
    sf::Sprite scoreboardSprite;
    sf::Texture levelSelectTexture;
    sf::Sprite levelSelectSprite;
    
    struct Button
    {
        sf::RectangleShape shape;
        std::string label;
        bool hovered;
    };
    
    Button playButton;
    Button scoresButton;
    Button loadButton;
    Button exitButton;
    Button forestButton;
    Button bossButton;
    
    // Load game UI
    std::vector<Button> saveSlotButtons;
    
    std::vector<int> topScores;
    MenuState currentState;
    LevelType selectedLevel;
    int selectedLoadSlot;
    SaveGameManager saveGameManager;
    
    bool isMouseOverButton(const Button& button, sf::Vector2f mousePos);
    void updateButtonHover(sf::Vector2f mousePos);
};

#endif
