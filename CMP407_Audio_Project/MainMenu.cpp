#include "MainMenu.h"
#include <iostream>

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

void Button::updateHover(sf::Vector2f mousePos)
{
	btnShape.setOutlineThickness(contains(mousePos) ? 2.f : 0.f);
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
		return MenuSelection::StartGame;
	if (m_settingsBtn.contains(mousePos))
		return MenuSelection::Settings;
	if (m_quitBtn.contains(mousePos))
		return MenuSelection::Quit;
}

void MainMenu::updateHover(sf::Vector2f mousePos)
{
	m_startBtn.updateHover(mousePos);
	m_settingsBtn.updateHover(mousePos);
	m_quitBtn.updateHover(mousePos);
}

void MainMenu::render(sf::RenderWindow& window)
{
	m_startBtn.draw(window);
	m_settingsBtn.draw(window);
	m_quitBtn.draw(window);
}

bool MainMenu::initAudio()
{
	if (!m_wwise.initSoundEngine(AKTEXT("Assets/Audio/CMP407_Dynamic_Audio_Project/GeneratedSoundBanks/Windows"))) 
	{
		std::cerr << "Could not initialise wwise - MainMenu.cpp - initAudio()" << std::endl;
		return false;
	}

	AkBankID bankID;
	if (AK::SoundEngine::LoadBank(AKTEXT("TestSoundBank"), bankID) != AK_Success) 
	{
		std::cerr << "Could not load soundbank - MainMenu.cpp - initAudio()" << std::endl;
		m_wwise.terminateSoundEngine();
		return false;
	}

	AK::SoundEngine::RegisterGameObj(m_menuAudioID);
	AK::SoundEngine::PostEvent(AKTEXT("BGM1_Loop"), m_menuAudioID);

	return true;
}

void MainMenu::stopAudio()
{
	// Until i make an event to stop the loop, use the stop all function.
	//AK::SoundEngine::PostEvent(AKTEXT("Stop_Loop"), m_menuAudioID);
	AK::SoundEngine::StopAll();
	AK::SoundEngine::UnregisterGameObj(m_menuAudioID);
	m_wwise.terminateSoundEngine();
}

void MainMenu::handleEvents(sf::RenderWindow& window, MenuSelection& selection)
{
}


