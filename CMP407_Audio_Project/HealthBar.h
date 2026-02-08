#pragma once
#include <SFML/Graphics.hpp>
class HealthBar
{
public:
	HealthBar(float maxHealth);
	~HealthBar(){}

	void Update(float currentHealth);
	void Render(sf::RenderWindow* window);

private:
	float _maxHealth;
	float _currentHealth;
	sf::Vector2f _barSize;
	sf::RectangleShape _backBar;
	sf::RectangleShape _frontBar;

};

