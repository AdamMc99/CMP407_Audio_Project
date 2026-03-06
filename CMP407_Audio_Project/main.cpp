/*
* CREDITS
* Start menu music pack - https://void1gaming.itch.io/free-game-menu-music-pack
*/

#include <SFML/Graphics.hpp>
#include "DynamicMain.h"
#include "MainMenu.h"
#include "WwiseWrapper.h"
#include <iostream>


int main() 
{
	sf::Font font;
	if (!font.openFromFile("Assets/Fonts/arial.ttf"))
		if (!font.openFromFile("arial.ttf"))
			printf("ERROR: Could not load font (arial.ttf) - main.cpp - main()");

	WwiseWrapper wwise;

	MainMenu menu(font, wwise);
	// Blocks program until player chooses
	MenuSelection selection = menu.run();

	// Player quit or closes window before selection
	if (selection == MenuSelection::Quit || selection == MenuSelection::None) return 0;

	// Game window only reached when selection == StartGame
	sf::RenderWindow window(sf::VideoMode({ 1000,1000 }), "Audio Project");
	window.setFramerateLimit(60);
	sf::Color clear_colour(135, 205, 250);

	DynamicMain dynamicMain(&window, &font);

	sf::Clock clock;
	float gameSpeed = 1.0f;


	while (window.isOpen()) 
	{
		float deltaTime = clock.restart().asSeconds() * gameSpeed;

		while (const std::optional event = window.pollEvent()) 
		{
			if (event->is<sf::Event::Closed>()) 
			{
				window.close();
			}
			if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) 
				{
					window.close();
				}
			}
		}

		dynamicMain.update(deltaTime);

		window.clear(dynamicMain.getBackgroundColour());
		dynamicMain.render();
		window.display();

	}
	return 0;
}