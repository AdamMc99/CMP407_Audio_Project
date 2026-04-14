#include "GameOverMenu.h"

GameOverMenu::GameOverMenu(sf::Font& font, WwiseWrapper& wwise)
    : BaseMenu(font, wwise, "Game Over", 400), m_timeText(font, "", 24)
{
    m_overlay.setFillColor(sf::Color(100, 0, 0, 150));
    m_title.setFillColor(sf::Color::Red);
    m_timeText.setFillColor(sf::Color::White);

    m_buttons.push_back(Button(font, "Restart", { 400.f, 400.f }, { 200.f, 50.f }, sf::Color(100, 255, 100), MenuSelection::Restart));
    m_buttons.push_back(Button(font, "Menu", { 400.f, 490.f }, { 200.f, 50.f }, sf::Color(100, 100, 255), MenuSelection::ReturnToMenu));
    m_buttons.push_back(Button(font, "Quit", { 400.f, 580.f }, { 200.f, 50.f }, sf::Color(255, 100, 100), MenuSelection::Quit));
}

void GameOverMenu::setSurvivalTime(float time) 
{
    m_timeText.setString("Survival Time: " + std::to_string(static_cast<int>(time)) + " seconds!");
    sf::FloatRect rect = m_timeText.getLocalBounds();
    m_timeText.setOrigin(rect.getCenter());
}

void GameOverMenu::render(sf::RenderWindow& window) 
{
    // Call parent class render
    BaseMenu::render(window);
    // Update time text's position
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    m_timeText.setPosition({ windowWidth / 2.f, windowHeight * 0.35f });
    // Draw overridden UI
    window.draw(m_timeText);
}