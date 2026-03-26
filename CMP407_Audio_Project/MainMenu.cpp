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
	m_startBtn(m_font, "Start Game", { 100.f, 30.f }, { 200.f, 50.f }, sf::Color(100, 255, 100)),
	m_settingsBtn(m_font, "Settings", { 100.f, 120.f }, { 200.f, 50.f }, sf::Color(100, 200, 255)),
	m_quitBtn(m_font, "Quit Game", { 100.f, 210.f }, { 200.f, 50.f }, sf::Color(255, 100, 100))
{
}

MenuSelection MainMenu::run()
{
	if (!initAudio()) return MenuSelection::None;

	sf::RenderWindow window(sf::VideoMode({ 400,300 }), "Main Menu");
	window.setFramerateLimit(60);

	MenuSelection selection = MenuSelection::None;

	while (window.isOpen())
	{
		sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		// Events -----
		while (const std::optional event = window.pollEvent()) 
		{
			if (event->is<sf::Event::Closed>()) 
			{
				window.close(); // Selection stays None
			}

			if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) 
			{
				if (mousePressed->button == sf::Mouse::Button::Left) 
				{
					if (m_startBtn.contains(mousePos)) 
					{
						selection = MenuSelection::StartGame;
						window.close();
					}
					else if(m_settingsBtn.contains(mousePos))
					{
						if (!m_settingsOpen) 
						{
							m_settingsOpen = true;
							// Settings is a seperate window.
							// Menu lloop resumes when it closes
							showSettings();
							m_settingsOpen = false;

							// Flush any events that were queued up while settings were open
							while (window.pollEvent().has_value()) {}
						}

					}
					else if (m_quitBtn.contains(mousePos)) 
					{
						selection = MenuSelection::Quit;
						window.close();
					}
				}
			}
			AK::SoundEngine::RenderAudio();
		}
		// Hover highlights
		m_startBtn.updateHover(mousePos);
		m_settingsBtn.updateHover(mousePos);
		m_quitBtn.updateHover(mousePos);

		// Render
		window.clear(sf::Color::Cyan);
		render(window);
		window.display();
	}

	// Menu music should stop here
	stopAudio();
	return selection;
}

void MainMenu::showSettings()
{
	sf::RenderWindow window(sf::VideoMode({ 400,300 }), "Settings");
	window.setFramerateLimit(60);

	// Title
	sf::Text title(m_font, "Settings", 28);
	title.setFillColor(sf::Color::White);
	{
		sf::FloatRect rect = title.getLocalBounds();
		title.setOrigin(rect.getCenter());
		title.setPosition({ 200.f,40.f });
	}

	// Placeholder stuff. Replace with actual settings later
	sf::Text placeholder(m_font, "{SETTINGS WILL GO HERE}", 16);
	placeholder.setFillColor(sf::Color(200, 200, 200));
	{
		sf::FloatRect rect = placeholder.getLocalBounds();
		placeholder.setOrigin(rect.getCenter());
		placeholder.setPosition({ 200.f,140.f });
	}

	// Back button
	Button backBtn(m_font, "Back", { 125.f,225.f }, { 150.f,45.f }, sf::Color(200, 100, 100));

	while (window.isOpen()) 
	{
		sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		while (const std::optional event = window.pollEvent()) 
		{
			if (event->getIf<sf::Event::Closed>()) 
			{
				window.close();
			}

			if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) 
			{
				if (mousePressed->button == sf::Mouse::Button::Left) 
				{
					if (backBtn.contains(mousePos)) 
					{
						window.close();
					}
				}
			}

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) 
			{
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) 
				{
					window.close();
				}
			}

			AK::SoundEngine::RenderAudio();
		}

		backBtn.updateHover(mousePos);

		window.clear(sf::Color(40, 40, 40));
		window.draw(title);
		window.draw(placeholder);
		backBtn.draw(window);
		window.display();
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


