#include "MainMenu.h"
#include <iostream>

MainMenu::MainMenu(sf::Font& font, WwiseWrapper& wwise) : BaseMenu(font, wwise, "Audio Project", 100) 
{
    // This menu doesn't need an overlay, so make it transparent
    m_overlay.setFillColor(sf::Color::Transparent);

    m_title.setPosition({ 500.f, 150.f });

    // Push the buttons into the BaseMenu vector
    m_buttons.push_back(Button(font, "Start Game", { 400.f, 300.f }, { 200.f, 50.f }, sf::Color(100, 255, 100), MenuSelection::StartGame));
    m_buttons.push_back(Button(font, "Settings", { 400.f, 420.f }, { 200.f, 50.f }, sf::Color(100, 200, 255), MenuSelection::Settings));
    m_buttons.push_back(Button(font, "Quit Game", { 400.f, 540.f }, { 200.f, 50.f }, sf::Color(255, 100, 100), MenuSelection::Quit));
}

bool MainMenu::initAudio()
{
    if (!m_wwise.initSoundEngine(AKTEXT("Assets/Audio/CMP407_Dynamic_Audio_Project/GeneratedSoundBanks/Windows"))) return false;

    AkBankID bankID;
    if (!m_wwise.loadBank(L"TestSoundBank.bnk")) return false;

    // BaseMenu already registered the UI Audio ID, so we just start the music
    playAudio();
    return true;
}

void MainMenu::playAudio()
{
    m_wwise.registerGameObject(m_menuBgmAudioID, "Menu BGM Audio");
    m_wwise.postEvent("Play_MainMenu_BGM1", m_menuBgmAudioID);
}

void MainMenu::stopAudio()
{
    m_wwise.stopAll(m_menuBgmAudioID);
    m_wwise.unregisterGameObject(m_menuBgmAudioID);
}