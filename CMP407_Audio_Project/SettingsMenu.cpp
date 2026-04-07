#include "SettingsMenu.h"

SettingsMenu::SettingsMenu(sf::Font& font, WwiseWrapper& wwise)
    : BaseMenu(font, wwise, "SETTINGS", 300), // 300 was your m_settingsUIAudioID
    m_masterText(font, "Master Volume", 24),
    m_musicText(font, "Music Volume", 24),
    m_sfxText(font, "SFX Volume", 24),
    m_masterSlider({ 400.f, 300.f }, { 200.f, 10.f }),
    m_musicSlider({ 400.f, 400.f }, { 200.f, 10.f }),
    m_sfxSlider({ 400.f, 500.f }, { 200.f, 10.f })
{
    // The BaseMenu handles the title, so we just setup our custom text labels
    m_masterText.setFillColor(sf::Color::White);
    m_masterText.setPosition({ 200.f, 290.f });

    m_musicText.setFillColor(sf::Color::White);
    m_musicText.setPosition({ 200.f, 390.f });

    m_sfxText.setFillColor(sf::Color::White);
    m_sfxText.setPosition({ 200.f, 490.f });

    // Push the Back button into the BaseMenu so it handles clicks and hovers for us!
    m_buttons.push_back(Button(font, "Back", { 400.f, 700.f }, { 200.f, 50.f }, sf::Color(255, 100, 100), MenuSelection::ReturnToMenu));
}

void SettingsMenu::updateSliders(sf::Vector2f mousePos, bool isMouseDown, WwiseWrapper& wwise)
{
    float masterVol = m_masterSlider.update(mousePos, isMouseDown);
    float musicVol = m_musicSlider.update(mousePos, isMouseDown);
    float sfxVol = m_sfxSlider.update(mousePos, isMouseDown);

    if (isMouseDown)
    {
        wwise.setRTPCValue("Master_Volume", masterVol, 0);
        wwise.setRTPCValue("Music_Volume", musicVol, 0);
        wwise.setRTPCValue("SFX_Volume", sfxVol, 0);
    }
}

void SettingsMenu::render(sf::RenderWindow& window)
{
    // 1. Let BaseMenu draw the overlay, the title, and the Back button
    BaseMenu::render(window);

    // 2. Draw our custom sliders on top
    window.draw(m_masterText);
    m_masterSlider.draw(window);

    window.draw(m_musicText);
    m_musicSlider.draw(window);

    window.draw(m_sfxText);
    m_sfxSlider.draw(window);
}