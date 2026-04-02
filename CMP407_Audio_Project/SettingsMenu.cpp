#include "SettingsMenu.h"
#include <iostream>

SettingsMenu::SettingsMenu(sf::Font& font, WwiseWrapper& wwise) : m_title(font, "SETTINGS", 60), m_wwise(wwise),
m_masterText(font, "Master Volume", 24), m_musicText(font, "Music Volume", 24), m_effectText(font, "SFX Volume", 24),
m_masterSlider({ 400.f,300.f }, { 200.f,10.f }), m_musicSlider({ 400.f,400.f }, { 200.f,10.f }), m_effectSlider({ 400.f, 500.f }, {200.f, 10.f}),
m_backBtn(font, "Back", { 400.f, 700.f }, {200.f, 50.f}, sf::Color(255,100,100))
{
	m_title.setFillColor(sf::Color::White);
	sf::FloatRect titleRect = m_title.getLocalBounds();
	m_title.setOrigin(titleRect.getCenter());
	m_title.setPosition({ 500.f, 150.f });

	m_masterText.setFillColor(sf::Color::White);
	m_masterText.setPosition({ 200.f, 290.f });
	m_musicText.setFillColor(sf::Color::White);
	m_musicText.setPosition({ 200.f, 390.f });
	m_effectText.setFillColor(sf::Color::White);
	m_effectText.setPosition({ 200.f, 490.f });

	m_wwise.registerGameObject(m_settingsUIAudioID, "Settings UI Audio");
}

MenuSelection SettingsMenu::checkClick(sf::Vector2f mousePos)
{
	if (m_backBtn.contains(mousePos)) 
	{
		m_wwise.postEvent("ButtonForward", m_settingsUIAudioID);
		return MenuSelection::ReturnToMenu;
	}
	return MenuSelection::None;
}

void SettingsMenu::updateHover(sf::Vector2f mousePos)
{
	if (m_backBtn.updateHover(mousePos)) 
		m_wwise.postEvent("ButtonHover", m_settingsUIAudioID);

}

void SettingsMenu::updateSliders(sf::Vector2f mousePos, bool isMouseDown, WwiseWrapper& wwise)
{
	float masterVol = m_masterSlider.update(mousePos, isMouseDown);
	float musicVol = m_musicSlider.update(mousePos, isMouseDown);
	float effectVol = m_effectSlider.update(mousePos, isMouseDown);

	if (isMouseDown) 
	{
		m_wwise.setRTCPValue("Master_Volume", masterVol, 0);
		wwise.setRTCPValue("Music_Volume", musicVol, 0);
		wwise.setRTCPValue("SFX_Volume", effectVol, 0);
	}
}

void SettingsMenu::render(sf::RenderWindow& window)
{
	window.draw(m_title);
	window.draw(m_masterText);
	m_masterSlider.draw(window);	
	window.draw(m_musicText);
	m_musicSlider.draw(window);	
	window.draw(m_effectText);
	m_effectSlider.draw(window);
	m_backBtn.draw(window);
}

