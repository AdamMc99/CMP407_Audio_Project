#include "DevTools.h"

DevTools::DevTools(sf::Font& font, DynamicMain& game) : m_game(game), m_devText(font, "", 16)
{
    m_devText.setFillColor(sf::Color::Yellow);
    m_devText.setPosition({ 10.f, 10.f }); // Top left corner
}

void DevTools::handleInput(const sf::Event& event)
{
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        // Only trigger if Control is held down
        if (keyPressed->control)
        {
            switch (keyPressed->scancode)
            {
            case sf::Keyboard::Scancode::Num1: // Ctrl + 1: God Mode
                m_godMode = !m_godMode;
                break;

            case sf::Keyboard::Scancode::Num2: // Ctrl + 2: Toggle Overlay
                m_showOverlay = !m_showOverlay;
                break;

            case sf::Keyboard::Scancode::H: // Ctrl + H: Heal Player
                // make sure m_player.addHealth() is accessible
                break;

            case sf::Keyboard::Scancode::K: // Ctrl + K: Kill all enemies
                // call something like m_game.clearEnemies() here
                break;
            }
        }
    }
}

void DevTools::render(sf::RenderWindow& window)
{
    if (!m_showOverlay) return;

    std::string info = "--- DEV TOOLS ---\n";
    info += "God Mode (Ctrl+1): " + std::string(m_godMode ? "ON" : "OFF") + "\n";

    m_devText.setString(info);
    window.draw(m_devText);
}