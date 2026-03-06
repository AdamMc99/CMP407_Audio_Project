#pragma once
#include <SFML/Graphics.hpp>
class HealthBar
{
public:
	HealthBar(float maxHealth);
	~HealthBar(){}

	void update(float currentHealth);
	void render(sf::RenderWindow* window);

private:
	float m_maxHealth;
	float m_currentHealth;
	sf::Vector2f m_barSize;
	sf::RectangleShape m_backBar;
	sf::RectangleShape m_frontBar;

};

