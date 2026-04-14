#pragma once
#include "BaseMenu.h"

class GameOverMenu : public BaseMenu {
public:
    GameOverMenu(sf::Font& font, WwiseWrapper& wwise);
    void setSurvivalTime(float time);
    void render(sf::RenderWindow& window) override;
private:
    sf::Text m_timeText;
};