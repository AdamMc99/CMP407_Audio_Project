#pragma once
#include <SFML/Graphics.hpp>
#include "DynamicMain.h"

class DevTools {
public:
    DevTools(sf::Font& font, DynamicMain& game);

    void handleInput(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);

    bool isGodModeActive() const { return m_godMode; }

private:
    DynamicMain& m_game;
    sf::Text m_devMenuText;     // The menu that appears when holding CTRL
    sf::Text m_debugStatsText;  // The text that appears when displaying debug info

    bool m_godMode = false;
    bool m_showDebugStats = false;
    float m_fps = 0.f;
};