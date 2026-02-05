#include <SFML/Graphics.hpp>

enum projectOptions{Dynamic, Procedural, Combo, None};

projectOptions ShowMenu()
{
	sf::RenderWindow window(sf::VideoMode({400, 300}), "Select Project");
	window.setFramerateLimit(60);

	sf::Font font;
	if (!font.openFromFile("Assets/arial.ttf")) 
		if (!font.openFromFile("arial.ttf"))
			printf("ERROR: Could not load font (arial.ttf) - main.cpp - ShowMenu");

	sf::RectangleShape dynamicBtn({ 200.f,50.f });
	dynamicBtn.setPosition({ 100.f, 50.f });
	dynamicBtn.setFillColor(sf::Color(100, 255, 100)); // Green
	sf::Text dynamicText(font, "Dynamic Audio", 20);
	dynamicText.setFillColor(sf::Color::Black);
	sf::FloatRect textRect = dynamicText.getLocalBounds();
	dynamicText.setOrigin(textRect.getCenter());
	dynamicText.setPosition({ dynamicBtn.getPosition().x + 100.f, dynamicBtn.getPosition().y + 25.f });

	sf::RectangleShape proceduralBtn({ 200.f, 50.f });
	proceduralBtn.setPosition({ 100.f, 160.f });
	proceduralBtn.setFillColor(sf::Color(100,200,255)); // Blue
	sf::Text proceduralText(font, "Procedural Audio", 20);
	textRect = proceduralText.getLocalBounds();
	proceduralText.setOrigin(textRect.getCenter());
	dynamicText.setPosition({ proceduralBtn.getPosition().x + 100.f, proceduralBtn.getPosition().y + 25.f });

	while (window.isOpen()) 
	{
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
					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

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
	}
}

int main() 
{

	projectOptions projectChoice = ShowMenu();

	sf::RenderWindow window(sf::VideoMode({ 640,480 }), "Audio Project Selection Screen");
	window.setFramerateLimit(60);
	sf::Color clear_colour(135, 205, 250);

	sf::Clock clock;
	float gameSpeed = 1.0f;


	while (window.isOpen()) 
	{
		float dt = clock.reset().asSeconds() * gameSpeed;

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

		window.clear(clear_colour);

		window.display();

	}

	return 0;
}