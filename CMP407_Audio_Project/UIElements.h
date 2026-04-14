#pragma once
#include <SFML/Graphics.hpp>

enum class MenuSelection { StartGame, Settings, Quit, Resume, ReturnToMenu, Restart, None };

class Button
{
public:
	Button(const sf::Font& font, const std::string& text, sf::Vector2f pos, sf::Vector2f size, sf::Color colour, MenuSelection action);

	MenuSelection getSelection() const { return m_action; }
	bool updateHover(sf::Vector2f mousePos);
	bool contains(sf::Vector2f mousePos) const;
	void draw(sf::RenderWindow& window) const;
private:
	sf::RectangleShape m_btnShape;
	sf::Text m_btnText;
	bool m_isHovered = false;
	MenuSelection m_action;
};

class Slider
{
public:
	Slider(sf::Vector2f pos, sf::Vector2f size);

	float update(sf::Vector2f mousePos, bool isMouseDown);
	void draw(sf::RenderWindow& window) const;
	void setPosition(sf::Vector2f pos);
	float getValue() const { return m_value; };

private:
	sf::RectangleShape m_track;
	sf::RectangleShape m_handle;
	sf::RectangleShape m_fill;

	float m_value = 100.f;
	bool m_isDragging = false;
};

class UIElements
{
};

