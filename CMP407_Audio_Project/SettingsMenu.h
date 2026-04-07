#pragma once
#include "BaseMenu.h"

class SettingsMenu : public BaseMenu {
public:
    SettingsMenu(sf::Font& font, WwiseWrapper& wwise);

    void updateSliders(sf::Vector2f mousePos, bool isMouseDown, WwiseWrapper& wwise);

    // Override the BaseMenu render function to add the sliders
    void render(sf::RenderWindow& window) override;

private:
    sf::Text m_masterText;
    Slider m_masterSlider;

    sf::Text m_musicText;
    Slider m_musicSlider;

    sf::Text m_sfxText;
    Slider m_sfxSlider;
};