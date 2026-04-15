#include "BaseMenu.h"

/// <summary>
/// Constructor to initialise a general menu background, centered title text, and menu audio objects. 
/// </summary>
BaseMenu::BaseMenu(sf::Font& font, WwiseWrapper& wwise, const std::string& titleText, uint64_t audioID)
    : m_font(font), m_wwise(wwise), m_title(font, titleText, 60), m_menuAudioID(audioID)
{
    m_overlay.setSize({ 1000.f, 1000.f });
    m_overlay.setFillColor(sf::Color(0, 0, 0, 150)); // Default dark transparent

    m_title.setFillColor(sf::Color::White);
    sf::FloatRect rect = m_title.getLocalBounds();
    m_title.setOrigin(rect.getCenter());
    m_title.setPosition({ 500.f, 250.f });

    // Link audio ID
    m_wwise.registerGameObject(m_menuAudioID, titleText + " Audio");
}

/// <summary>
/// Check if mouse clicked on any buttons and triggers sound effect.
/// </summary>
/// <returns>Plays sound and activates action.</returns>
MenuSelection BaseMenu::checkClick(sf::Vector2f mousePos)
{
    for (auto& btn : m_buttons)
    {
        if (btn.contains(mousePos))
        {
            m_wwise.postEvent("ButtonForward", m_menuAudioID);
            return btn.getSelection();
        }
    }
    return MenuSelection::None;
}

/// <summary>
/// Highlights buttons when mouse hovers over it and plays sound effect.
/// </summary>
void BaseMenu::updateHover(sf::Vector2f mousePos)
{
    bool hoveredAny = false;
    for (auto& btn : m_buttons)
    {
        if (btn.updateHover(mousePos))
        {
            hoveredAny = true;
        }
    }
    if (hoveredAny) m_wwise.postEvent("ButtonHover", m_menuAudioID);
}

/// <summary>
/// Render common menu components such as background shade, title, and buttons.
/// </summary>
void BaseMenu::render(sf::RenderWindow& window)
{
    float winWidth = static_cast<float>(window.getSize().x);
    float winHeight = static_cast<float>(window.getSize().y);

    // Force overlay to match the current window size
    m_overlay.setSize({ winWidth, winHeight });

    // Keep the title centered horizontally, 25% down from the top
    m_title.setPosition({ winWidth / 2.f, winHeight * 0.25f });

    window.draw(m_overlay);
    window.draw(m_title);

    for (const auto& btn : m_buttons)
    {
        btn.draw(window);
    }
}