#include "PauseMenu.h"

PauseMenu::PauseMenu(sf::Font& font, WwiseWrapper& wwise) : BaseMenu(font, wwise, "PAUSED", 500)
{
    m_buttons.push_back(Button(font, "Resume", { 400.f, 400.f }, { 200.f, 50.f }, sf::Color(100, 255, 100), MenuSelection::Resume));
    m_buttons.push_back(Button(font, "Main Menu", { 400.f, 490.f }, { 200.f, 50.f }, sf::Color(100, 100, 255), MenuSelection::ReturnToMenu));
    m_buttons.push_back(Button(font, "Quit Game", { 400.f, 580.f }, { 200.f, 50.f }, sf::Color(255, 100, 100), MenuSelection::Quit));
}