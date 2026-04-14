/*
*  ------ PROGRAM NOTES ------
*  - Naming Conventions
*   > Private member variables start with "m_". Eg "m_playerShape"
*   > Function names start with lower case
*   > Error messages should end with location. Eg "Error message - filename.cpp"
*   > BGM wwise registrations should be "(state) BGM Audio". Eg "Menu BGM Audio"
* 
*  - Game objects should be able to handle changes in window size
* 
* https://opengameart.org/content/metalstone-textures background texture
*/

#include <SFML/Graphics.hpp>
#include "DynamicMain.h"
#include "MainMenu.h"
#include "PauseMenu.h"
#include "SettingsMenu.h"
#include "GameOverMenu.h"
#include "WwiseWrapper.h"
#include "DevTools.h"
#include <iostream>

enum class GameState {
    MainMenu,
    Settings,
    Game,
    Pause,
    GameOver,
    Exit,
    None
};

int main()
{
    sf::Font font;
    if (!font.openFromFile("Assets/Fonts/arial.ttf")) {
        if (!font.openFromFile("arial.ttf")) {
            std::cerr << "ERROR: Could not load font (arial.ttf) - main.cpp\n";
            return -1;
        }
    }

    WwiseWrapper wwise;

    sf::RenderWindow window(sf::VideoMode({ 1000, 1000 }), "Dynamic Audio - Menu");
    window.setFramerateLimit(60);

    MainMenu menu(font, wwise);
    menu.initAudio();
    DynamicMain dynamicMain(&window, font, wwise);
    PauseMenu pauseMenu(font, wwise);
    SettingsMenu settingsMenu(font, wwise);
    GameOverMenu gameoverMenu(font, wwise);
    DevTools devTools(font, dynamicMain);


    GameState currentState = GameState::MainMenu;
    sf::Clock clock;
    float gameSpeed = 1.0f;

    while (window.isOpen() && currentState != GameState::Exit)
    {
        float deltaTime = clock.restart().asSeconds() * gameSpeed;
        sf::View uiView(sf::FloatRect({ 0.f,0.f }, { static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) }));
        window.setView(uiView);

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        // --- EVENT HANDLING ---
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                currentState = GameState::Exit;
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                {
                    if (currentState == GameState::Game) // If in game show pause screen
                    {
                        currentState = GameState::Pause;
                        window.setMouseCursorVisible(true);
                    }
                    else if (currentState == GameState::Pause) // If on pause screen return to game
                    {
                        currentState = GameState::Game;
                        window.setMouseCursorVisible(!dynamicMain.isCursorHidden());
                    }
                    else if (currentState == GameState::Settings) // If in settings return to menu
                    {
                        currentState = GameState::MainMenu;
                    }
                    else if (currentState == GameState::GameOver) // If in gameover return to menu
                    {
                        currentState = GameState::MainMenu;
                    }
                    else {
                        currentState = GameState::Exit;
                    }
                }
            }

            if (currentState == GameState::Game) 
            {
                devTools.handleInput(*event);
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == sf::Mouse::Button::Left)
                {
                    if (currentState == GameState::MainMenu) // ---- MAIN MENU
                    {
                        MenuSelection selection = menu.checkClick(mousePos);

                        if (selection == MenuSelection::StartGame) // Start game
                        {
                            // Reset game
                            dynamicMain.reset();
                            // Audio transition
                            menu.stopAudio();
                            dynamicMain.playAudio();
                            //Change state
                            currentState = GameState::Game;
                        }
                        else if (selection == MenuSelection::Settings) // Open settings
                        { 
                            currentState = GameState::Settings;
                        }
                        else if (selection == MenuSelection::Quit) // Quit game
                        {
                            currentState = GameState::Exit;
                        }
                    }
                    else if (currentState == GameState::Settings) // ---- SETTINGS
                    {
                        MenuSelection selection = settingsMenu.checkClick(mousePos);
                        if (selection == MenuSelection::ReturnToMenu)
                        {
                            currentState = GameState::MainMenu;
                        }
                    }
                    else if (currentState == GameState::Pause) // ---- PAUSE MENU
                    {
                        MenuSelection selection = pauseMenu.checkClick(mousePos);
                        if (selection == MenuSelection::Resume) // Return to game
                        {
                            currentState = GameState::Game;
                            window.setMouseCursorVisible(!dynamicMain.isCursorHidden());
                        }
                        else if (selection == MenuSelection::ReturnToMenu) // Return to main menu
                        {
                            dynamicMain.stopAudio();
                            menu.playAudio();
                            currentState = GameState::MainMenu;
                        }
                        else if (selection == MenuSelection::Quit) // Quit game
                        {
                            currentState = GameState::Exit;
                        }
                    }
                    else if (currentState == GameState::GameOver) // ---- GAME OVER
                    {
                        MenuSelection selection = gameoverMenu.checkClick(mousePos);
                        if (selection == MenuSelection::Restart) 
                        {
                            dynamicMain.reset(); 
                            dynamicMain.playAudio(); 
                            currentState = GameState::Game;
                            window.setMouseCursorVisible(true);
                        }
                        else if (selection == MenuSelection::ReturnToMenu) 
                        {
                            dynamicMain.stopAudio();
                            menu.playAudio();
                            currentState = GameState::MainMenu;
                        }
                        else if (selection == MenuSelection::Quit) 
                        {
                            currentState = GameState::Exit;
                        }
                    }
                }
            }
        }

        // --- UPDATE & RENDER ---
        window.clear(sf::Color(135, 205, 250)); // Default clear color

        if (currentState == GameState::Settings) 
        {
            bool isMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            settingsMenu.updateSliders(mousePos, isMouseDown, wwise);
        }

        switch (currentState)
        {
        case GameState::MainMenu:
            window.setTitle("Dynamic Audio - Menu");
            menu.updateHover(mousePos); 
            menu.render(window);
            break;

        case GameState::Settings:
            //window.clear(sf::Color(40, 40, 40));
            window.setTitle("Dynamic Audio - Settings");
            settingsMenu.updateHover(mousePos);
            settingsMenu.render(window);
            break;

        case GameState::Game:
            window.setTitle("Dynamic Audio - Game");
            dynamicMain.update(deltaTime);
            devTools.update(deltaTime);
            window.clear(dynamicMain.getBackgroundColour()); // Override clear color for game
            dynamicMain.render();
            devTools.render(window);

            if (dynamicMain.isPlayerDead() && !devTools.isGodModeActive())
            {
                currentState = GameState::GameOver;
                gameoverMenu.setSurvivalTime(dynamicMain.getPlayTime());
                window.setMouseCursorVisible(true);
                dynamicMain.stopAudio();
            }
            break;
        
        case GameState::Pause:
            window.setTitle("Dynamic Audio - Paused");
            pauseMenu.updateHover(mousePos);
            window.clear(dynamicMain.getBackgroundColour());
            dynamicMain.render();       // Draw frozen game
            pauseMenu.render(window);   // Draw pause menu
            break;

        case GameState::GameOver:
            window.setTitle("Dynamic Audio - GAMEOVER!");
            gameoverMenu.updateHover(mousePos);
            gameoverMenu.render(window);
            break;

        case GameState::Exit:
            window.close();
            break;
        }

        // --- AUDIO ENGINE ---
        // Render audio once per frame regardless of state
        wwise.update();

        window.display();
    }

    wwise.terminateSoundEngine();
    return 0;
}