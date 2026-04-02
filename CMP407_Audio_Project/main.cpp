#include <SFML/Graphics.hpp>
#include "DynamicMain.h"
#include "MainMenu.h"
#include "PauseMenu.h"
#include "SettingsMenu.h"
#include "WwiseWrapper.h"
#include <iostream>

enum class AppState {
    MainMenu,
    Settings,
    Game,
    Pause,
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

    sf::RenderWindow window(sf::VideoMode({ 1000, 1000 }), "Audio Project");
    window.setFramerateLimit(60);

    MainMenu menu(font, wwise);
    menu.initAudio();
    DynamicMain dynamicMain(&window, font, wwise);
    PauseMenu pauseMenu(font, wwise);
    SettingsMenu settingsMenu(font, wwise);


    AppState currentState = AppState::MainMenu;
    sf::Clock clock;
    float gameSpeed = 1.0f;

    while (window.isOpen() && currentState != AppState::Exit)
    {
        float deltaTime = clock.restart().asSeconds() * gameSpeed;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        // --- EVENT HANDLING ---
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                currentState = AppState::Exit;
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                {
                    if (currentState == AppState::Game) // If in game show pause screen
                    {
                        currentState = AppState::Pause;
                        window.setMouseCursorVisible(true);
                    }
                    else if (currentState == AppState::Pause) // If on pause screen return to game
                    {
                        currentState = AppState::Game;
                        window.setMouseCursorVisible(!dynamicMain.isCursorHidden());
                    }
                    else if (currentState == AppState::Settings) // If in settings return to menu
                    {
                        currentState = AppState::MainMenu;
                    }
                    else {
                        currentState = AppState::Exit;
                    }
                }
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == sf::Mouse::Button::Left)
                {
                    if (currentState == AppState::MainMenu) // ---- MAIN MENU
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
                            currentState = AppState::Game;
                        }
                        else if (selection == MenuSelection::Settings) // Open settings
                        { 
                            currentState = AppState::Settings;
                        }
                        else if (selection == MenuSelection::Quit) // Quit game
                        {
                            currentState = AppState::Exit;
                        }
                    }
                    else if (currentState == AppState::Settings) // ---- SETTINGS
                    {
                        MenuSelection selection = settingsMenu.checkClick(mousePos);
                        if (selection == MenuSelection::ReturnToMenu)
                        {
                            currentState = AppState::MainMenu;
                        }
                    }
                    else if (currentState == AppState::Pause) // ---- PAUSE MENU
                    {
                        MenuSelection selection = pauseMenu.checkClick(mousePos);
                        if (selection == MenuSelection::Resume) // Return to game
                        {
                            currentState = AppState::Game;
                            window.setMouseCursorVisible(!dynamicMain.isCursorHidden());
                        }
                        else if (selection == MenuSelection::ReturnToMenu) // Return to main menu
                        {
                            dynamicMain.stopAudio();
                            menu.playAudio();
                            currentState = AppState::MainMenu;
                        }
                        else if (selection == MenuSelection::Quit) // Quit game
                        {
                            currentState = AppState::Exit;
                        }
                    }
                }
            }
        }

        // --- UPDATE & RENDER ---
        window.clear(sf::Color(135, 205, 250)); // Default clear color

        if (currentState == AppState::Settings) 
        {
            bool isMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            settingsMenu.updateSliders(mousePos, isMouseDown, wwise);
        }

        switch (currentState)
        {
        case AppState::MainMenu:
            menu.updateHover(mousePos); 
            menu.render(window);
            break;

        case AppState::Settings:
            //window.clear(sf::Color(40, 40, 40));
            settingsMenu.updateHover(mousePos);
            settingsMenu.render(window);
            break;

        case AppState::Game:
            dynamicMain.update(deltaTime);
            window.clear(dynamicMain.getBackgroundColour()); // Override clear color for game
            dynamicMain.render();
            break;
        
        case AppState::Pause:
            pauseMenu.updateHover(mousePos);

            window.clear(dynamicMain.getBackgroundColour());
            dynamicMain.render();       // Draw frozen game
            pauseMenu.render(window);   // Draw pause menu
            break;

        case AppState::Exit:
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