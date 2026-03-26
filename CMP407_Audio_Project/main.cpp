#include <SFML/Graphics.hpp>
#include "DynamicMain.h"
#include "MainMenu.h"
#include "WwiseWrapper.h"
#include <iostream>

enum class AppState {
    MainMenu,
    Settings,
    Game,
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
    DynamicMain dynamicMain(&window, &font, wwise);

    menu.initAudio();

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
                    // If in game or settings, go back to menu. If in menu, exit.
                    if (currentState == AppState::Game) {
                        dynamicMain.stopAudio();
                        menu.playAudio();
                        currentState = AppState::MainMenu;
                    }
                    else if (currentState == AppState::Settings) 
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
                    if (currentState == AppState::MainMenu)
                    {
                        MenuSelection selection = menu.checkClick(mousePos);

                        if (selection == MenuSelection::StartGame) 
                        {
                            menu.stopAudio();
                            dynamicMain.playAudio();
                            currentState = AppState::Game;
                        }
                        else if (selection == MenuSelection::Settings) 
                        { 
                            currentState = AppState::Settings;
                        }
                        else if (selection == MenuSelection::Quit) 
                        {
                            currentState = AppState::Exit;
                        }
                    }
                    else if (currentState == AppState::Settings)
                    {
                        // if (settingsBackBtnClicked) currentState = AppState::MainMenu;
                    }
                }
            }
        }

        // --- UPDATE & RENDER ---
        window.clear(sf::Color(135, 205, 250)); // Default clear color

        switch (currentState)
        {
        case AppState::MainMenu:
            menu.updateHover(mousePos); 
            menu.render(window);
            break;

        case AppState::Settings:
            // Render settings UI here
            break;

        case AppState::Game:
            dynamicMain.update(deltaTime);
            window.clear(dynamicMain.getBackgroundColour()); // Override clear color for game
            dynamicMain.render();
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