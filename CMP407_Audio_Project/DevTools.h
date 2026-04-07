#pragma once
#include <SFML/Graphics.hpp>
#include "DynamicMain.h"

class DevTools {
public:
    DevTools(sf::Font& font, DynamicMain& game);

    void handleInput(const sf::Event& event);

    void render(sf::RenderWindow& window);

    bool isGodModeActive() const { return m_godMode; }

private:
    DynamicMain& m_game;
    sf::Text m_devText;

    bool m_godMode = false;
    bool m_showOverlay = true; // Toggle to hide the dev text
};