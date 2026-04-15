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
*  - Heartbeat sound effect RTCP is commented out in DynamicMain as the BGM 
*     and HB dont mix very well. Uncomment and mute BGM in settings to showcase.
* 
*  - Thanks to
*   > https://opengameart.org/content/metalstone-textures -Background texture
*   > https://pixabay.com/sound-effects/search/heartbeat/ -BRVHRTZ - Heartbeat SFX
*   > https://www.cambridge-mt.com/ms3/mtk/               -Babe Grand 'Aguille Rouge' - Game BGM
*   > Soundly - Button SFX
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

// Each possible state in the game
enum class GameState {
    MainMenu,
    Settings,
    Game,
    Pause,
    GameOver,
    Exit,
    None
};


/// <summary>
/// Main entry point. Initialises the window, audio, menus, and game objects,
/// then runs the main game loop events, updates and rendering.
/// </summary>
int main()
{
    // load the basic font used for the game
    sf::Font font;
    if (!font.openFromFile("Assets/Fonts/arial.ttf")) {
        if (!font.openFromFile("arial.ttf")) {
            std::cerr << "ERROR: Could not load font (arial.ttf) - main.cpp\n";
            return -1;
        }
    }

    // Create the game window
    sf::RenderWindow window(sf::VideoMode({ 1000, 1000 }), "Dynamic Audio - Menu");
    window.setFramerateLimit(60);

    // Initialise the audio wrapper
    WwiseWrapper wwise;

    // Create menus and game objects
    MainMenu menu(font, wwise);
    menu.initAudio();
    DynamicMain dynamicMain(&window, font, wwise);
    PauseMenu pauseMenu(font, wwise);
    SettingsMenu settingsMenu(font, wwise);
    GameOverMenu gameoverMenu(font, wwise);
    DevTools devTools(font, dynamicMain);

    // Set initial game state and timer
    GameState currentState = GameState::MainMenu;
    sf::Clock clock;
    float gameSpeed = 1.0f;

    // Main game loop
    while (window.isOpen() && currentState != GameState::Exit)
    {
        // Calculate time passed since previous frame
        float deltaTime = clock.restart().asSeconds() * gameSpeed;

        // Reset view to UI
        sf::View uiView(sf::FloatRect({ 0.f,0.f }, { static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) }));
        window.setView(uiView);

        // Get the current mouse position
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        // --- EVENT HANDLING ---
        // Process user input and window events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                currentState = GameState::Exit;
            }

            // Check if a key was pressed
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // Escape key
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

            // Handle dev tool inputs during gameplay
            if (currentState == GameState::Game) 
            {
                devTools.handleInput(*event);
            }

            // Check if mouse button was clicked
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == sf::Mouse::Button::Left)
                {
                    // Handle clicks on main menu
                    if (currentState == GameState::MainMenu)
                    {
                        MenuSelection selection = menu.checkClick(mousePos);

                        if (selection == MenuSelection::StartGame)
                        {
                            // Reset game
                            dynamicMain.reset();
                            // Audio transition
                            menu.stopAudio();
                            dynamicMain.playAudio();
                            //Change state
                            currentState = GameState::Game;
                        }
                        else if (selection == MenuSelection::Settings)
                        { 
                            currentState = GameState::Settings;
                        }
                        else if (selection == MenuSelection::Quit)
                        {
                            currentState = GameState::Exit;
                        }
                    }
                    // Handle clicks on settings menu
                    else if (currentState == GameState::Settings)
                    {
                        MenuSelection selection = settingsMenu.checkClick(mousePos);
                        if (selection == MenuSelection::ReturnToMenu)
                        {
                            currentState = GameState::MainMenu;
                        }
                    }
                    // Handle clicks on pasue menu
                    else if (currentState == GameState::Pause)
                    {
                        MenuSelection selection = pauseMenu.checkClick(mousePos);
                        if (selection == MenuSelection::Resume)
                        {
                            currentState = GameState::Game;
                            window.setMouseCursorVisible(!dynamicMain.isCursorHidden());
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
                    // Handle clicks on game over menu
                    else if (currentState == GameState::GameOver)
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
        // Clear window with a light blue colour
        window.clear(sf::Color(135, 205, 250));

        // Update audio sliders if settings menu is open
        if (currentState == GameState::Settings) 
        {
            bool isMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            settingsMenu.updateSliders(mousePos, isMouseDown, wwise);
        }

        // Handle specific logic and drawing for current game state
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

            // Check if player has died
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
        // Process audio updates
        wwise.update();
        // draw the current frame to the screen
        window.display();
    }

    // Clean uo audio engine before quitting
    wwise.terminateSoundEngine();
    return 0;
}