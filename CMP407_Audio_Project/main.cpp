#include <SFML/Graphics.hpp>
#include "DynamicMain.h"
#include <iostream>

#include <AK/SoundEngine/Common/AkMemoryMgr.h> // Memory manager interface
#include <AK/SoundEngine/Common/AkMemoryMgrModule.h> // Default memory manager
#include <AK/SoundEngine/Common/IAkStreamMgr.h> // streaming manager
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h> // sound engine
#include <AK/Tools/Common/AkPlatformFuncs.h> // Thread defines

#ifndef AK_OPTIMIZED
#include <Ak/Comm/AkCommunication.h>
#endif

CAkFilePackageLowLevelIODeferred g_lowLevelIO;

bool InitSoundEngine()
{
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success) 
	{
		std::cout << "AK memory manager failed to initalise! -- main.cpp" << std::endl;
		return false;
	}

	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	if(!AK::StreamMgr::Create(stmSettings))
	{
		std::cout << "AK stream manager failed to initalise! -- main.cpp" << std::endl;
		return false;
	}

	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	if (g_lowLevelIO.Init(deviceSettings) != AK_Success) 
	{
		std::cout << "AK streaming device failed to initalise! -- main.cpp" << std::endl;
		return false;
	}


	
	
	return true;
}

enum projectOptions{Dynamic, Procedural, Combo, None};

projectOptions ShowMenu(sf::Font font)
{
	sf::RenderWindow window(sf::VideoMode({400, 300}), "Select Project");
	window.setFramerateLimit(60);

	sf::RectangleShape dynamicBtn({ 200.f,50.f });
	dynamicBtn.setPosition({ 100.f, 50.f });
	dynamicBtn.setFillColor(sf::Color(100, 255, 100)); // Green
	sf::Text dynamicText(font, "Dynamic Audio", 20);
	dynamicText.setFillColor(sf::Color::Black);
	sf::FloatRect textRect = dynamicText.getLocalBounds();
	dynamicText.setOrigin(textRect.getCenter());
	dynamicText.setPosition({ dynamicBtn.getPosition().x + 100.f, dynamicBtn.getPosition().y + 25.f});

	sf::RectangleShape proceduralBtn({ 200.f, 50.f });
	proceduralBtn.setPosition({ 100.f, 160.f });
	proceduralBtn.setFillColor(sf::Color(100,200,255)); // Blue
	sf::Text proceduralText(font, "Procedural Audio", 20);
	proceduralText.setFillColor(sf::Color::Black);
	textRect = proceduralText.getLocalBounds();
	proceduralText.setOrigin(textRect.getCenter());
	proceduralText.setPosition({ proceduralBtn.getPosition().x + 100.f, proceduralBtn.getPosition().y + 25.f});

	while (window.isOpen()) 
	{
		sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		
		while (const std::optional event = window.pollEvent()) 
		{
			if (event->is<sf::Event::Closed>()) 
			{
				window.close();
				return None;
			}

			if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) 
			{
				if (mousePress->button == sf::Mouse::Button::Left) 
				{
					//sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

					if (dynamicBtn.getGlobalBounds().contains(mousePos)) 
					{
						window.close();
						return Dynamic;
					}
					if(proceduralBtn.getGlobalBounds().contains(mousePos))
					{
						window.close();
						return Procedural;
					}
				}
			}
		}

		if (dynamicBtn.getGlobalBounds().contains(mousePos)) dynamicBtn.setOutlineThickness(2);
		else dynamicBtn.setOutlineThickness(0);

		if (proceduralBtn.getGlobalBounds().contains(mousePos)) proceduralBtn.setOutlineThickness(2);
		else proceduralBtn.setOutlineThickness(0);

		window.clear(sf::Color::Cyan);
		window.draw(dynamicBtn);
		window.draw(dynamicText);
		window.draw(proceduralBtn);
		window.draw(proceduralText);
		window.display();
	}
	return None;
}

int main() 
{
	if (InitSoundEngine() == true) 
	{
		std::cout << "Audio system initialisation complete!" << std::endl;
	}
	else 
	{
		std::cout << "Audio system initialisation failed! -- main.cpp" << std::endl;
	}

	sf::Font font;
	if (!font.openFromFile("Assets/Fonts/arial.ttf"))
		if (!font.openFromFile("arial.ttf"))
			printf("ERROR: Could not load font (arial.ttf) - main.cpp - ShowMenu");

	projectOptions projectChoice = ShowMenu(font);

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

		if (projectChoice == Dynamic) 
		{
			dynamicMain.Update(deltaTime);
			clear_colour = dynamicMain.GetBackgroundColour();
			
			window.clear(clear_colour);
			dynamicMain.Render();
			window.display();
		}
		if (projectChoice == Procedural) 
		{
			clear_colour = sf::Color::Red;
			window.clear(clear_colour);
			window.display();
		}

	}

	return 0;
}