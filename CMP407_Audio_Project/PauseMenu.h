#pragma once
#include <SFML/Graphics.hpp>
#include "MainMenu.h"
class PauseMenu
{
public:
	PauseMenu(sf::Font& font);
	MenuSelection checkClick(sf::Vector2f mousePos);
	void updateHover(sf::Vector2f mousePos);
	void render(sf::RenderWindow& window);

private:
	sf::RectangleShape m_overlay;
	sf::Text m_title;
	Button m_resumeBtn;
	Button m_returnBtn;
	Button m_quitBtn;
};

