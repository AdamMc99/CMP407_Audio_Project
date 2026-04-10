#include "DevTools.h"

DevTools::DevTools(sf::Font& font, DynamicMain& game) :
    m_game(game), m_devMenuText(font, "", 16), m_debugStatsText(font, "", 14)
{
    // Setup bottom left dev menu text
    sf::FloatRect rect = m_devMenuText.getLocalBounds();
    m_devMenuText.setOrigin({rect.size.x, 0});
    m_devMenuText.setFillColor(sf::Color::Yellow);

    // Setup top right debug stats text
    m_debugStatsText.setFillColor(sf::Color::White);
    m_debugStatsText.setOutlineColor(sf::Color::Black);
    m_debugStatsText.setOutlineThickness(1.f);
}

void DevTools::handleInput(const sf::Event& event)
{
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        // Only trigger the actions if Control is held down
        if (keyPressed->control)
        {
            switch (keyPressed->scancode)
            {
            case sf::Keyboard::Scancode::Num1: // Ctrl + 1: display debug info
                m_showDebugStats = !m_showDebugStats;
                break;

            case sf::Keyboard::Scancode::Num2: // Ctrl + 2: god mode
                m_godMode = !m_godMode;
                break;

            case sf::Keyboard::Scancode::Num3: // Ctrl + 3: Kill all enemies
                m_game.killAll();
                break;
            case sf::Keyboard::Scancode::Num4: // Ctrl + 4: Add 10 player health
                m_game.addHealth();
                break;
            case sf::Keyboard::Scancode::Num5: // Ctrl + 5: Remove 10 player health
                m_game.removeHealth();
                break;
            case sf::Keyboard::Scancode::Num6: // Ctrl + 6: Remove 10 intesnity
                m_game.adjustIntensity(-1);
                break;
            case sf::Keyboard::Scancode::Num7: // Ctrl + 7 Add 10 intensity
                m_game.adjustIntensity(1);
                break;            
            case sf::Keyboard::Scancode::Num8: // Ctrl + 8 Toggle intensity pause
                m_game.toggleIntensity();
                break;
            }
        }
    }
}

void DevTools::update(float dt) 
{
    if (!m_showDebugStats) return;

    m_fps = 1.f / dt;

    std::string info = "";
    info += "FPS: " + std::to_string(static_cast<int>(m_fps)) + "\n";
    info += "Enemies: " + std::to_string(m_game.getEnemyCount()) + "\n";
    info += "Defended: " + std::to_string(m_game.getEnemiesDefended()) + "\n";
    info += "Darkness: " + std::to_string(m_game.getDarknessFactor()) + "\n";
    info += "Spawn Rate: " + std::to_string(m_game.getCurrentSpawnRate()) + "\n";
    info += "Intensity: " + std::to_string(m_game.getIntensity()) + "\n";

    m_debugStatsText.setString(info);

    // Anchor to top right corner 
    sf::FloatRect bounds = m_debugStatsText.getLocalBounds();
    m_debugStatsText.setOrigin({ bounds.size.x, 0.f });
    m_debugStatsText.setPosition({ 990.f, 10.f });
}

void DevTools::render(sf::RenderWindow& window)
{
    if (m_showDebugStats)
    {
        // Get the width of the window right now
        float windowWidth = static_cast<float>(window.getSize().x);

        // 10 pixels from the right edge
        sf::FloatRect bounds = m_debugStatsText.getLocalBounds();
        m_debugStatsText.setOrigin({ bounds.size.x, 0.f });
        m_debugStatsText.setPosition({ windowWidth - 10.f, 10.f });

        window.draw(m_debugStatsText);
    }

    // Draw Dev Menu ONLY while holding Ctrl
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RControl))
    {
        std::string info = "--- DEV TOOLS ---\n";
        info += "Debug Stats (Ctrl+1): " + std::string(m_showDebugStats ? "ON" : "OFF") + "\n";
        info += "God Mode (Ctrl+2): " + std::string(m_godMode ? "ON" : "OFF") + "\n";
        info += "Kill All (Ctrl+3) \n";
        info += "Adjust health (Ctrl+4/5) \n";
        info += "Adjust intensity (Ctrl+6/7) \n";
        info += "Toggle pause intensity (Ctrl+8) \n";

        m_devMenuText.setString(info);

        sf::FloatRect bounds = m_devMenuText.getLocalBounds();
        float windowHeight = static_cast<float>(window.getSize().y);
        m_devMenuText.setOrigin({ 0.f, bounds.size.y });
        m_devMenuText.setPosition({ 10.f, windowHeight - 10.f });
        window.draw(m_devMenuText);
    }
}