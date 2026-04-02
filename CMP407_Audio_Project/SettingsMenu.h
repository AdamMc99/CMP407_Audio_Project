#pragma once
#include <SFML/Graphics.hpp>
#include "MainMenu.h"
#include "WwiseWrapper.h"

class SettingsMenu
{
public:
	SettingsMenu(sf::Font& font, WwiseWrapper& wwise);

	MenuSelection checkClick(sf::Vector2f mousePos);
	void updateHover(sf::Vector2f mousePos);
	void updateSliders(sf::Vector2f mousePos, bool isMouseDown, WwiseWrapper& wwise);
	void render(sf::RenderWindow& window);

private:
	sf::Text m_title;

	sf::Text m_masterText;
	Slider m_masterSlider;

	sf::Text m_musicText;
	Slider m_musicSlider;

	sf::Text m_effectText;
	Slider m_effectSlider;

	Button m_backBtn;

	WwiseWrapper& m_wwise;
	static constexpr uint64_t m_settingsUIAudioID = 200;

};

