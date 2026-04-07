#include "GameOverMenu.h"

GameOverMenu::GameOverMenu(sf::Font& font, WwiseWrapper& wwise)
    : BaseMenu(font, wwise, "Game Over", 400)
{
    // We can override the overlay color in the child class!
    m_overlay.setFillColor(sf::Color(100, 0, 0, 150));
    m_title.setFillColor(sf::Color::Red);

    m_buttons.push_back(Button(font, "Restart", { 400.f, 400.f }, { 200.f, 50.f }, sf::Color(100, 255, 100), MenuSelection::Restart));
    m_buttons.push_back(Button(font, "Menu", { 400.f, 490.f }, { 200.f, 50.f }, sf::Color(100, 100, 255), MenuSelection::ReturnToMenu));
    m_buttons.push_back(Button(font, "Quit", { 400.f, 580.f }, { 200.f, 50.f }, sf::Color(255, 100, 100), MenuSelection::Quit));
}