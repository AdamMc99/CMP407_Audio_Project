#include "SettingsMenu.h"

/// <summary>
/// Constructor to initialise volume sliders and labels for settings.
/// </summary>
SettingsMenu::SettingsMenu(sf::Font& font, WwiseWrapper& wwise)
    : BaseMenu(font, wwise, "SETTINGS", 300),
    m_masterText(font, "Master Volume", 24),
    m_musicText(font, "Music Volume", 24),
    m_sfxText(font, "SFX Volume", 24),
    m_masterSlider({ 400.f, 300.f }, { 200.f, 10.f }),
    m_musicSlider({ 400.f, 400.f }, { 200.f, 10.f }),
    m_sfxSlider({ 400.f, 500.f }, { 200.f, 10.f })
{
    // Configure text appearance and position
    // BaseMenu handles title text
    m_masterText.setFillColor(sf::Color::White);
    m_masterText.setPosition({ 200.f, 290.f });

    m_musicText.setFillColor(sf::Color::White);
    m_musicText.setPosition({ 200.f, 390.f });

    m_sfxText.setFillColor(sf::Color::White);
    m_sfxText.setPosition({ 200.f, 490.f });

    // Add back button
    m_buttons.push_back(Button(font, "Back", { 400.f, 700.f }, { 200.f, 50.f }, sf::Color(255, 100, 100), MenuSelection::ReturnToMenu));
}

/// <summary>
/// Check if mouse is modifying the sliders and updates the volume.
/// </summary>
void SettingsMenu::updateSliders(sf::Vector2f mousePos, bool isMouseDown, WwiseWrapper& wwise)
{
    float masterVol = m_masterSlider.update(mousePos, isMouseDown);
    float musicVol = m_musicSlider.update(mousePos, isMouseDown);
    float sfxVol = m_sfxSlider.update(mousePos, isMouseDown);

    // Apply new volume to audio engine if mouse is pressed
    if (isMouseDown)
    {
        wwise.setRTPCValue("Master_Volume", masterVol, 0);
        wwise.setRTPCValue("Music_Volume", musicVol, 0);
        wwise.setRTPCValue("SFX_Volume", sfxVol, 0);
    }
}

/// <summary>
/// Draws the background, text, and sliders to the window.
/// </summary>
void SettingsMenu::render(sf::RenderWindow& window)
{
    /// Draw BaseMenu elements
    BaseMenu::render(window);

    float centerX = static_cast<float>(window.getSize().x) / 2.f;

    // Position text and sliders
    m_masterText.setPosition({ centerX - 300.f, 290.f });
    m_masterSlider.setPosition({ centerX - 100.f, 300.f });

    m_musicText.setPosition({ centerX - 300.f, 390.f });
    m_musicSlider.setPosition({ centerX - 100.f, 400.f });

    m_sfxText.setPosition({ centerX - 300.f, 490.f });
    m_sfxSlider.setPosition({ centerX - 100.f, 500.f });

    // Draw settings elements
    window.draw(m_masterText);
    m_masterSlider.draw(window);

    window.draw(m_musicText);
    m_musicSlider.draw(window);

    window.draw(m_sfxText);
    m_sfxSlider.draw(window);
}