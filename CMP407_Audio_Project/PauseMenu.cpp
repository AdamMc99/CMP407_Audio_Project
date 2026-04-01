#include "PauseMenu.h"

PauseMenu::PauseMenu(sf::Font& font) :
	m_title(font, "PAUSED", 60),
	m_resumeBtn(font, "Resume", { 400.f,400.f }, { 200.f,50.f }, sf::Color(100,255,100)),
	m_returnBtn(font, "Main Menu", { 400.f,490.f }, { 200.f,50.f }, sf::Color(100,100,255)),
	m_quitBtn(font, "Quit Game", { 400.f, 580.f }, {200.f, 50.f}, sf::Color(255,100,100))
{
	m_overlay.setSize({ 1000.f,1000.f });
	m_overlay.setFillColor(sf::Color(0, 0, 0, 150)); // 150 Alpha makes is semi-transparent

	m_title.setFillColor(sf::Color::White);
	sf::FloatRect rect = m_title.getLocalBounds();
	m_title.setOrigin(rect.getCenter());
	m_title.setPosition({ 500.f, 250.f });
}

MenuSelection PauseMenu::checkClick(sf::Vector2f mousePos) 
{
	if (m_resumeBtn.contains(mousePos)) return MenuSelection::Resume;
	if (m_returnBtn.contains(mousePos)) return MenuSelection::ReturnToMenu;
	if (m_quitBtn.contains(mousePos)) return MenuSelection::Quit;
	return MenuSelection::None;
}

void PauseMenu::updateHover(sf::Vector2f mousePos) 
{
	m_resumeBtn.updateHover(mousePos);
	m_returnBtn.updateHover(mousePos);
	m_quitBtn.updateHover(mousePos);
}

void PauseMenu::render(sf::RenderWindow& window) 
{
	window.draw(m_overlay);
	window.draw(m_title);
	m_resumeBtn.draw(window);
	m_returnBtn.draw(window);
	m_quitBtn.draw(window);
}