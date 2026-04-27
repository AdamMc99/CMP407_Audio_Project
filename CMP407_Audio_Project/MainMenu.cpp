#include "MainMenu.h"
#include <iostream>

/// <summary>
/// Constructor to set up main menu buttons and graphics.
/// </summary>
MainMenu::MainMenu(sf::Font& font, WwiseWrapper& wwise) : BaseMenu(font, wwise, "Audio Project", 100) 
{
    // Menu doesnt need an overlay, make it transparent
    m_overlay.setFillColor(sf::Color::Transparent);

    m_title.setPosition({ 500.f, 150.f });

    // Add buttons to menu screen
    m_buttons.push_back(Button(font, "Start Game", { 400.f, 300.f }, { 200.f, 50.f }, sf::Color(100, 255, 100), MenuSelection::StartGame));
    m_buttons.push_back(Button(font, "Settings", { 400.f, 420.f }, { 200.f, 50.f }, sf::Color(100, 200, 255), MenuSelection::Settings));
    m_buttons.push_back(Button(font, "Quit Game", { 400.f, 540.f }, { 200.f, 50.f }, sf::Color(255, 100, 100), MenuSelection::Quit));
}


/// <summary>
/// Starts state's audio.
/// </summary>
void MainMenu::playAudio()
{
    m_wwise.registerGameObject(m_menuBgmAudioID, "Menu BGM Audio");
    m_wwise.postEvent("Play_MainMenu_BGM1", m_menuBgmAudioID);
}

/// <summary>
/// Stops all state active audio and removes game object from sound engine.
/// </summary>
void MainMenu::stopAudio()
{
    m_wwise.stopAll(m_menuBgmAudioID);
    m_wwise.unregisterGameObject(m_menuBgmAudioID);
}