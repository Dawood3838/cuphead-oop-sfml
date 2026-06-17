#include "menu.h"
#include <fstream>
#include <algorithm>

Menu::Menu() : currentState(SHOWING_MENU), selectedLevel(FOREST_FOLLIES), selectedLoadSlot(-1)
{
    // Load background image for main menu
    if (!backgroundTexture.loadFromFile("Data/frontpage.png"))
    {
        // If image fails to load, create a solid color background
        sf::Image image;
        image.create(1136, 896, sf::Color(50, 50, 100));
        backgroundTexture.loadFromImage(image);
    }
    
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setPosition(0, 0);
    
    // Load scoreboard image for scores screen
    if (!scoreboardTexture.loadFromFile("Data/scoregemini.png"))
    {
        // If image fails to load, create a dark background
        sf::Image image;
        image.create(1136, 896, sf::Color(30, 30, 50));
        scoreboardTexture.loadFromImage(image);
    }
    
    scoreboardSprite.setTexture(scoreboardTexture);
    scoreboardSprite.setPosition(0, 0);
    
    // Load level select background image
    if (!levelSelectTexture.loadFromFile("Data/playback.png"))
    {
        // If image fails to load, create a solid color background
        sf::Image image;
        image.create(1136, 896, sf::Color(80, 40, 20));
        levelSelectTexture.loadFromImage(image);
    }
    
    levelSelectSprite.setTexture(levelSelectTexture);
    levelSelectSprite.setPosition(0, 0);
    levelSelectSprite.setScale(1136.0f / levelSelectTexture.getSize().x, 
                               896.0f / levelSelectTexture.getSize().y);
    
    // Setup Play button
    playButton.shape.setSize(sf::Vector2f(250, 80));
    playButton.shape.setPosition(443, 280);
    playButton.shape.setFillColor(sf::Color(100, 200, 100));
    playButton.label = "PLAY";
    playButton.hovered = false;
    
    // Setup Scores button
    scoresButton.shape.setSize(sf::Vector2f(250, 80));
    scoresButton.shape.setPosition(443, 395);
    scoresButton.shape.setFillColor(sf::Color(100, 150, 200));
    scoresButton.label = "SCORES";
    scoresButton.hovered = false;
    
    // Setup Load button
    loadButton.shape.setSize(sf::Vector2f(250, 80));
    loadButton.shape.setPosition(443, 510);
    loadButton.shape.setFillColor(sf::Color(150, 100, 200));
    loadButton.label = "LOAD GAME";
    loadButton.hovered = false;
    
    // Setup Exit button
    exitButton.shape.setSize(sf::Vector2f(250, 80));
    exitButton.shape.setPosition(443, 625);
    exitButton.shape.setFillColor(sf::Color(200, 100, 100));
    exitButton.label = "EXIT";
    exitButton.hovered = false;
    
    // Setup Forest Follies button
    forestButton.shape.setSize(sf::Vector2f(250, 80));
    forestButton.shape.setPosition(200, 400);
    forestButton.shape.setFillColor(sf::Color(34, 139, 34));
    forestButton.label = "FOREST";
    forestButton.hovered = false;
    
    // Setup Boss Level button
    bossButton.shape.setSize(sf::Vector2f(250, 80));
    bossButton.shape.setPosition(686, 400);
    bossButton.shape.setFillColor(sf::Color(139, 0, 0));
    bossButton.label = "BOSS LEVEL";
    bossButton.hovered = false;
    
    // Initialize save slot buttons for load game screen
    for (int i = 0; i < 5; i++)
    {
        Button slotButton;
        slotButton.shape.setSize(sf::Vector2f(800, 100));
        slotButton.shape.setPosition(168, 250 + i * 110);
        slotButton.shape.setFillColor(sf::Color(100, 100, 150));
        slotButton.label = "SAVE SLOT " + std::to_string(i + 1);
        slotButton.hovered = false;
        saveSlotButtons.push_back(slotButton);
    }
    
    // Load scores from file
    loadScores();
}

Menu::~Menu()
{
}

void Menu::loadScores()
{
    topScores.clear();
    std::ifstream file("Data/scores.txt");
    
    if (file.is_open())
    {
        int score;
        while (file >> score && topScores.size() < 5)
        {
            topScores.push_back(score);
        }
        file.close();
    }
    
    // Ensure we have exactly 5 scores
    while (topScores.size() < 5)
    {
        topScores.push_back(0);
    }
}

void Menu::saveScores()
{
    std::ofstream file("Data/scores.txt");
    
    if (file.is_open())
    {
        for (int score : topScores)
        {
            file << score << "\n";
        }
        file.close();
    }
}

void Menu::addScore(int score)
{
    topScores.push_back(score);
    std::sort(topScores.begin(), topScores.end(), std::greater<int>());
    
    if (topScores.size() > 5)
    {
        topScores.resize(5);
    }
    
    saveScores();
}

bool Menu::isMouseOverButton(const Button& button, sf::Vector2f mousePos)
{
    return button.shape.getGlobalBounds().contains(mousePos);
}

void Menu::updateButtonHover(sf::Vector2f mousePos)
{
    if (currentState == SHOWING_MENU)
    {
        playButton.hovered = isMouseOverButton(playButton, mousePos);
        scoresButton.hovered = isMouseOverButton(scoresButton, mousePos);
        loadButton.hovered = isMouseOverButton(loadButton, mousePos);
        exitButton.hovered = isMouseOverButton(exitButton, mousePos);
        
        // Brighten hovered buttons
        playButton.shape.setFillColor(playButton.hovered ? sf::Color(150, 255, 150) : sf::Color(100, 200, 100));
        scoresButton.shape.setFillColor(scoresButton.hovered ? sf::Color(150, 200, 255) : sf::Color(100, 150, 200));
        loadButton.shape.setFillColor(loadButton.hovered ? sf::Color(200, 150, 255) : sf::Color(150, 100, 200));
        exitButton.shape.setFillColor(exitButton.hovered ? sf::Color(255, 150, 150) : sf::Color(200, 100, 100));
    }
    else if (currentState == SHOWING_LEVEL_SELECT)
    {
        forestButton.hovered = isMouseOverButton(forestButton, mousePos);
        bossButton.hovered = isMouseOverButton(bossButton, mousePos);
        
        // Brighten hovered buttons
        forestButton.shape.setFillColor(forestButton.hovered ? sf::Color(100, 200, 100) : sf::Color(34, 139, 34));
        bossButton.shape.setFillColor(bossButton.hovered ? sf::Color(200, 100, 100) : sf::Color(139, 0, 0));
    }
    else if (currentState == SHOWING_LOAD_GAME)
    {
        for (size_t i = 0; i < saveSlotButtons.size(); i++)
        {
            saveSlotButtons[i].hovered = isMouseOverButton(saveSlotButtons[i], mousePos);
            saveSlotButtons[i].shape.setFillColor(
                saveSlotButtons[i].hovered ? sf::Color(150, 150, 200) : sf::Color(100, 100, 150)
            );
        }
    }
}

Menu::MenuState Menu::handleMouseClick(sf::Vector2f mousePos)
{
    updateButtonHover(mousePos);
    
    if (currentState == SHOWING_MENU)
    {
        if (playButton.hovered)
        {
            currentState = SHOWING_LEVEL_SELECT;
            return SHOWING_LEVEL_SELECT;
        }
        else if (scoresButton.hovered)
        {
            currentState = SHOWING_SCORES;
            return SHOWING_SCORES;
        }
        else if (loadButton.hovered)
        {
            currentState = SHOWING_LOAD_GAME;
            return SHOWING_LOAD_GAME;
        }
        else if (exitButton.hovered)
        {
            currentState = EXIT_GAME;
            return EXIT_GAME;
        }
    }
    else if (currentState == SHOWING_SCORES)
    {
        currentState = SHOWING_MENU;
        return SHOWING_MENU;
    }
    else if (currentState == SHOWING_LEVEL_SELECT)
    {
        if (forestButton.hovered)
        {
            selectedLevel = FOREST_FOLLIES;
            currentState = START_GAME;
            return START_GAME;
        }
        else if (bossButton.hovered)
        {
            selectedLevel = BOSS_LEVEL;
            currentState = START_GAME;
            return START_GAME;
        }
    }
    else if (currentState == SHOWING_LOAD_GAME)
    {
        for (size_t i = 0; i < saveSlotButtons.size(); i++)
        {
            if (saveSlotButtons[i].hovered)
            {
                // Only load if slot is used (not empty)
                if (saveGameManager.isSlotUsed(i))
                {
                    selectedLoadSlot = i;
                    currentState = LOAD_GAME;
                    return LOAD_GAME;
                }
                // If slot is empty, do nothing
                break;
            }
        }
    }
    
    return currentState;
}

void Menu::update()
{
    // Update logic if needed
}

void Menu::draw(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    
    if (currentState == SHOWING_MENU)
    {
        // Draw main menu background
        window.draw(backgroundSprite);
        
        // Draw buttons
        window.draw(playButton.shape);
        window.draw(scoresButton.shape);
        window.draw(loadButton.shape);
        window.draw(exitButton.shape);
        
        // Draw button text
        sf::Font font;
        if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"))
        {
            sf::Text playText(playButton.label, font, 40);
            playText.setPosition(500, 290);
            playText.setFillColor(sf::Color::Black);
            window.draw(playText);
            
            sf::Text scoresText(scoresButton.label, font, 40);
            scoresText.setPosition(470, 405);
            scoresText.setFillColor(sf::Color::Black);
            window.draw(scoresText);
            
            sf::Text loadText(loadButton.label, font, 35);
            loadText.setPosition(460, 520);
            loadText.setFillColor(sf::Color::Black);
            window.draw(loadText);
            
            sf::Text exitText(exitButton.label, font, 40);
            exitText.setPosition(520, 635);
            exitText.setFillColor(sf::Color::Black);
            window.draw(exitText);
        }
    }
    else if (currentState == SHOWING_LEVEL_SELECT)
    {
        // Draw level select background
        levelSelectSprite.setPosition(0, 0);
        window.draw(levelSelectSprite);
        
        // Draw level select buttons
        window.draw(forestButton.shape);
        window.draw(bossButton.shape);
        
        // Draw button text
        sf::Font font;
        if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"))
        {
            // Title
            sf::Text titleText("SELECT LEVEL", font, 50);
            titleText.setStyle(sf::Text::Bold);
            titleText.setFillColor(sf::Color::White);
            titleText.setOutlineThickness(2.0f);
            titleText.setOutlineColor(sf::Color::Black);
            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setPosition(568 - titleBounds.width / 2, 100);
            window.draw(titleText);
            
            // Forest button text
            sf::Text forestText(forestButton.label, font, 35);
            forestText.setStyle(sf::Text::Bold);
            forestText.setFillColor(sf::Color::White);
            forestText.setOutlineThickness(2.0f);
            forestText.setOutlineColor(sf::Color::Black);
            sf::FloatRect forestBounds = forestText.getLocalBounds();
            forestText.setPosition(325 - forestBounds.width / 2, 410);
            window.draw(forestText);
            
            // Boss button text
            sf::Text bossText(bossButton.label, font, 35);
            bossText.setStyle(sf::Text::Bold);
            bossText.setFillColor(sf::Color::White);
            bossText.setOutlineThickness(2.0f);
            bossText.setOutlineColor(sf::Color::Black);
            sf::FloatRect bossBounds = bossText.getLocalBounds();
            bossText.setPosition(811 - bossBounds.width / 2, 410);
            window.draw(bossText);
            
            // Back instruction
            sf::Text backText("Click to select a level", font, 25);
            backText.setStyle(sf::Text::Bold);
            backText.setFillColor(sf::Color::White);
            backText.setOutlineThickness(1.5f);
            backText.setOutlineColor(sf::Color::Black);
            backText.setPosition(380, 750);
            window.draw(backText);
        }
    }
    else if (currentState == SHOWING_SCORES)
    {
        // Draw scoreboard background
        scoreboardSprite.setPosition(0, 0);
        window.draw(scoreboardSprite);
        
        // Draw scores screen
        sf::Font font;
        if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"))
        {
            // Position scores in the middle of the screen
            float centerX = 568;  // Middle of 1136 width
            float headingY = 250;  // Above the scores
            float scoresStartY = 350;  // Below heading
            
            // Title "TOP 5 SCORES" with white outline and black fill
            sf::Text titleText("TOP 5 SCORES", font, 50);
            titleText.setStyle(sf::Text::Bold);
            titleText.setFillColor(sf::Color::Black);
            titleText.setOutlineThickness(2.0f);
            titleText.setOutlineColor(sf::Color::White);
            
            // Center the text
            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setPosition(centerX - titleBounds.width / 2, headingY);
            window.draw(titleText);
            
            for (size_t i = 0; i < topScores.size(); ++i)
            {
                std::string scoreString = std::to_string(i + 1) + ". " + std::to_string(topScores[i]);
                sf::Text scoreText(scoreString, font, 40);
                scoreText.setStyle(sf::Text::Bold);
                scoreText.setFillColor(sf::Color::Black);
                scoreText.setOutlineThickness(2.0f);
                scoreText.setOutlineColor(sf::Color::White);
                
                // Center each score text
                sf::FloatRect scoreBounds = scoreText.getLocalBounds();
                scoreText.setPosition(centerX - scoreBounds.width / 2, scoresStartY + i * 50);
                window.draw(scoreText);
            }
            
            sf::Text backText("Press ESC or Click to go back", font, 25);
            backText.setStyle(sf::Text::Bold);
            backText.setFillColor(sf::Color::Black);
            backText.setOutlineThickness(1.5f);
            backText.setOutlineColor(sf::Color::White);
            backText.setPosition(350, 800);
            window.draw(backText);
        }
    }
    else if (currentState == SHOWING_LOAD_GAME)
    {
        // Draw dark background for load game
        window.clear(sf::Color(40, 40, 60));
        
        // Reload save slot info to refresh the display
        saveGameManager.loadSaveSlotInfo();
        
        // Draw save slot buttons
        for (size_t i = 0; i < saveSlotButtons.size(); i++)
        {
            window.draw(saveSlotButtons[i].shape);
        }
        
        // Draw UI text
        sf::Font font;
        if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"))
        {
            // Title
            sf::Text titleText("SELECT SAVE TO LOAD", font, 50);
            titleText.setStyle(sf::Text::Bold);
            titleText.setFillColor(sf::Color::White);
            titleText.setOutlineThickness(2.0f);
            titleText.setOutlineColor(sf::Color::Black);
            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setPosition(568 - titleBounds.width / 2, 80);
            window.draw(titleText);
            
            // Draw save slot info text
            for (size_t i = 0; i < saveSlotButtons.size(); i++)
            {
                sf::Text slotText("SLOT " + std::to_string(i + 1) + ": EMPTY", font, 30);
                slotText.setFillColor(sf::Color::White);
                slotText.setPosition(190, 270 + i * 110);
                window.draw(slotText);
                
                // Check if slot is used and display info
                if (saveGameManager.isSlotUsed(i))
                {
                    SaveGameSlot slot = saveGameManager.getSaveSlot(i);
                    
                    // Format: SLOT N: LEVEL - Score: XXX, Time: XXm XXs
                    std::string slotInfo = "SLOT " + std::to_string(i + 1) + ": " + slot.levelName;
                    slotInfo += " | Score: " + std::to_string(slot.score);
                    
                    int minutes = (int)(slot.playtime / 60.0f);
                    int seconds = (int)(slot.playtime) % 60;
                    slotInfo += " | Time: " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
                    
                    sf::Text slotInfoText(slotInfo, font, 22);
                    slotInfoText.setFillColor(sf::Color::Yellow);
                    slotInfoText.setPosition(190, 270 + i * 110);
                    window.draw(slotInfoText);
                    
                    // Show timestamp
                    sf::Text timestampText(slot.timestamp, font, 18);
                    timestampText.setFillColor(sf::Color(200, 200, 200));
                    timestampText.setPosition(200, 295 + i * 110);
                    window.draw(timestampText);
                }
            }
            
            sf::Text backText("Click to load or Click outside to go back", font, 22);
            backText.setStyle(sf::Text::Bold);
            backText.setFillColor(sf::Color::White);
            backText.setOutlineThickness(1.5f);
            backText.setOutlineColor(sf::Color::Black);
            backText.setPosition(300, 800);
            window.draw(backText);
        }
    }
    
    window.display();
}
