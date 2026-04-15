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
/// Initialise the sound engine and load the required audio files for the menu.
/// </summary>
/// <returns>True if successful initialisation.</returns>
bool MainMenu::initAudio()
{
    if (!m_wwise.initSoundEngine(AKTEXT("Assets/Audio/CMP407_Dynamic_Audio_Project/GeneratedSoundBanks/Windows"))) return false;

    AkBankID bankID;
    if (!m_wwise.loadBank(L"TestSoundBank.bnk")) return false;

    // BaseMenu registered the UI Audio ID, so just start the music
    playAudio();
    return true;
}

/// <summary>
/// Starts state's audio.
/// </summary>
void MainMenu::playAudio()
{
    m_wwise.registerGameObject(m_menuBgmAudioID, "Menu BGM Audio");
    m_wwise.postEvent("Play_MainMenu_BGM1", m_menuBgmAudioID);
    std::cout << "SECTION REACHED";
}

/// <summary>
/// Stops all state active audio and removes game object from sound engine.
/// </summary>
void MainMenu::stopAudio()
{
    m_wwise.stopAll(m_menuBgmAudioID);
    m_wwise.unregisterGameObject(m_menuBgmAudioID);
}