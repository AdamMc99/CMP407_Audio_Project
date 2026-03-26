#pragma once
#include <SFML/Graphics.hpp>
#include "WwiseWrapper.h"


enum class MenuSelection {StartGame, Settings, Quit, None};

struct Button 
{
	sf::RectangleShape btnShape;
	sf::Text btnText;

	Button(const sf::Font& font, const std::string& text, sf::Vector2f pos, sf::Vector2f size, sf::Color colour);
	void updateHover(sf::Vector2f mousePos);
	bool contains(sf::Vector2f mousePos) const;
	void draw(sf::RenderWindow& window) const;
};

class MainMenu
{
public:
	MainMenu(sf::Font& font, WwiseWrapper& wwise);

	MenuSelection checkClick(sf::Vector2f mousePos);
	void updateHover(sf::Vector2f mousePos);
	
	// wwise helpers
	bool initAudio();
	void stopAudio();

	// UI helpers
	void handleEvents(sf::RenderWindow& window, MenuSelection& selection);
	void render(sf::RenderWindow& window);

private:
	// data
	sf::Font& m_font;
	WwiseWrapper& m_wwise;
	bool m_settingsOpen = false;

	Button m_startBtn;
	Button m_settingsBtn;
	Button m_quitBtn;

	// Wwise ID for menu music
	static constexpr uint64_t m_menuAudioID = 1;
};

