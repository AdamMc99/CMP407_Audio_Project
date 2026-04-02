#include "MainMenu.h"
#include <iostream>
#include "PauseMenu.h"

// --------- BUTTON ------------

Button::Button(const sf::Font& font, const std::string& text, sf::Vector2f pos, sf::Vector2f size, sf::Color colour) : btnText(font, text, 20)
{
	btnShape.setSize(size);
	btnShape.setPosition(pos);
	btnShape.setFillColor(colour);
	btnShape.setOutlineColor(sf::Color::Black);
	btnShape.setOutlineThickness(0);

	btnText = sf::Text(font, text, 20);
	btnText.setFillColor(sf::Color::Black);
	sf::FloatRect rect = btnText.getLocalBounds();
	btnText.setOrigin(rect.getCenter());
	btnText.setPosition({ pos.x + size.x / 2.f, pos.y + size.y / 2.f });
}

bool Button::updateHover(sf::Vector2f mousePos)
{
	bool isCurrentlyHovering = contains(mousePos);

	if (isCurrentlyHovering && !isHovered) 
	{
		isHovered = true;
		btnShape.setOutlineThickness(2.0f);
		return true;
	}
	else if (!isCurrentlyHovering && isHovered) 
	{
		isHovered = false;
		btnShape.setOutlineThickness(0.0f);
	}
	return false;
}

bool Button::contains(sf::Vector2f mousePos) const
{
	return btnShape.getGlobalBounds().contains(mousePos);
}

void Button::draw(sf::RenderWindow& window) const
{
	window.draw(btnShape);
	window.draw(btnText);
}

// ------------- Slider ----------------
Slider::Slider(sf::Vector2f pos, sf::Vector2f size) 
{
	m_track.setSize(size);
	m_track.setPosition(pos);
	m_track.setFillColor(sf::Color(100, 100, 100));

	m_handle.setSize({ 15.f, size.y + 10.f });
	m_handle.setOrigin({ 7.5,5.f });
	m_handle.setPosition({pos.x + size.x, pos.y});
	m_handle.setFillColor(sf::Color::White);
}

float Slider::update(sf::Vector2f mousePos, bool isMouseDown) 
{
	if (isMouseDown && m_handle.getGlobalBounds().contains(mousePos)) 
	{
		m_isDragging = true;
	}
	else if (!isMouseDown) 
	{
		m_isDragging = false;
	}

	if (m_isDragging) 
	{
		float newX = std::max(m_track.getPosition().x, std::min(mousePos.x, m_track.getPosition().x + m_track.getSize().x));
		m_handle.setPosition({ newX, m_handle.getPosition().y });

		m_value = ((newX - m_track.getPosition().x) / m_track.getSize().x) * 100.f;
	}
	return m_value;
}

void Slider::draw(sf::RenderWindow& window) const
{
	window.draw(m_track);
	window.draw(m_handle);
}

// -------------- MENU -----------------
MainMenu::MainMenu(sf::Font& font, WwiseWrapper& wwise) : m_font(font), m_wwise(wwise),
	m_startBtn(m_font, "Start Game", { 400, 30.f }, { 200.f, 50.f }, sf::Color(100, 255, 100)),
	m_settingsBtn(m_font, "Settings", { 400.f, 120.f }, { 200.f, 50.f }, sf::Color(100, 200, 255)),
	m_quitBtn(m_font, "Quit Game", { 400.f, 210.f }, { 200.f, 50.f }, sf::Color(255, 100, 100))
{
}

MenuSelection MainMenu::checkClick(sf::Vector2f mousePos)
{
	if (m_startBtn.contains(mousePos))
	{
		m_wwise.postEvent("ButtonForward", m_menuUIAudioID);
		return MenuSelection::StartGame;
	}
	if (m_settingsBtn.contains(mousePos))
	{
		m_wwise.postEvent("ButtonForward", m_menuUIAudioID);
		return MenuSelection::Settings;
	}
	if (m_quitBtn.contains(mousePos))
	{
		return MenuSelection::Quit;
	}

	return MenuSelection::None;
}

void MainMenu::updateHover(sf::Vector2f mousePos)
{
	if (m_startBtn.updateHover(mousePos) ||
		m_settingsBtn.updateHover(mousePos) ||
		m_quitBtn.updateHover(mousePos)) 
	{
		m_wwise.postEvent("ButtonHover", m_menuUIAudioID);
	}
}

void MainMenu::render(sf::RenderWindow& window)
{
	m_startBtn.draw(window);
	m_settingsBtn.draw(window);
	m_quitBtn.draw(window);
}

bool MainMenu::initAudio()
{
	if (!m_wwise.initSoundEngine(AKTEXT("Assets/Audio/CMP407_Dynamic_Audio_Project/GeneratedSoundBanks/Windows"))) return false;

	AkBankID bankID;
	if (!m_wwise.loadBank(L"TestSoundBank.bnk")) return false;

	m_wwise.registerGameObject(m_menuUIAudioID, "Menu UI Audio");

	playAudio();

	return true;
}

void MainMenu::playAudio() 
{
	m_wwise.registerGameObject(m_menuAudioID, "Menu BGM Audio");
	m_wwise.postEvent("BGM1_Loop", m_menuAudioID);
}

void MainMenu::stopAudio()
{
	m_wwise.stopAll(m_menuAudioID);
	m_wwise.unregisterGameObject(m_menuAudioID);
}

void MainMenu::handleEvents(sf::RenderWindow& window, MenuSelection& selection)
{
}


