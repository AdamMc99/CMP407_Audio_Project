#pragma once
#include <SFML/Graphics.hpp>
#include "WwiseWrapper.h"
#include "UIElements.h"
#include <vector>

class BaseMenu
{
public:
	BaseMenu(sf::Font& font, WwiseWrapper& wwise, const std::string& titleText, uint64_t audioID);

	// Virtual functions should allow child classes to overwite them if needed
	virtual void updateHover(sf::Vector2f mousePos);
	virtual MenuSelection checkClick(sf::Vector2f mousePos);
	virtual void render(sf::RenderWindow& window);

protected: // Protected should allow children to access but other classes cant
	WwiseWrapper& m_wwise;
	sf::Font& m_font;

	sf::Text m_title;
	sf::RectangleShape m_overlay;

	// Move all buttons into a vector instead of naming individually
	std::vector<Button> m_buttons;

	uint64_t m_menuAudioID;
};

