#include "UIElements.h"

// --------- BUTTON ------------

Button::Button(const sf::Font& font, const std::string& text, sf::Vector2f pos, sf::Vector2f size, sf::Color colour, MenuSelection action) 
	: m_btnText(font, text, 20), m_action(action)
{
	m_btnShape.setSize(size);
	m_btnShape.setPosition(pos);
	m_btnShape.setFillColor(colour);
	m_btnShape.setOutlineColor(sf::Color::Black);
	m_btnShape.setOutlineThickness(0);

	m_btnText = sf::Text(font, text, 20);
	m_btnText.setFillColor(sf::Color::Black);
	sf::FloatRect rect = m_btnText.getLocalBounds();
	m_btnText.setOrigin(rect.getCenter());
	m_btnText.setPosition({ pos.x + size.x / 2.f, pos.y + size.y / 2.f });
}

bool Button::updateHover(sf::Vector2f mousePos)
{
	bool isCurrentlyHovering = contains(mousePos);

	if (isCurrentlyHovering && !m_isHovered)
	{
		m_isHovered = true;
		m_btnShape.setOutlineThickness(2.0f);

		return true;
	}
	else if (!isCurrentlyHovering && m_isHovered)
	{
		m_isHovered = false;
		m_btnShape.setOutlineThickness(0.0f);
	}
	return false;
}

bool Button::contains(sf::Vector2f mousePos) const
{
	return m_btnShape.getGlobalBounds().contains(mousePos);
}

void Button::draw(sf::RenderWindow& window) const
{
	// You can dynamically center the button's X position based on the window
	float winWidth = static_cast<float>(window.getSize().x);

	sf::RectangleShape& shape = const_cast<sf::RectangleShape&>(m_btnShape);
	sf::Text& text = const_cast<sf::Text&>(m_btnText);

	// Keep the original Y position, but center the X
	shape.setPosition({ (winWidth / 2.f) - (shape.getSize().x / 2.f), shape.getPosition().y });

	// Recalculate text position to match the new shape position
	text.setPosition({ shape.getPosition().x + (shape.getSize().x / 2.f), shape.getPosition().y + (shape.getSize().y / 2.f) });

	window.draw(shape);
	window.draw(text);
}

// ------------- Slider ----------------
Slider::Slider(sf::Vector2f pos, sf::Vector2f size)
{
	m_track.setPosition(pos);
	m_track.setSize(size);
	m_track.setFillColor(sf::Color(100, 100, 100));

	m_fill.setPosition(pos);
	m_fill.setSize(size);
	m_fill.setFillColor(sf::Color(40, 40, 40));

	m_handle.setSize({ 15.f, size.y + 10.f });
	m_handle.setOrigin({ 7.5,5.f });
	m_handle.setPosition({ pos.x + size.x, pos.y });
	m_handle.setFillColor(sf::Color::White);
}

float Slider::update(sf::Vector2f mousePos, bool isMouseDown)
{
	if (isMouseDown && m_handle.getGlobalBounds().contains(mousePos))
	{
		m_isDragging = true;
	}
	else if (!isMouseDown)
	{
		m_isDragging = false;
	}

	if (m_isDragging)
	{
		float newX = std::max(m_track.getPosition().x, std::min(mousePos.x, m_track.getPosition().x + m_track.getSize().x));
		m_handle.setPosition({ newX, m_handle.getPosition().y });

		m_value = ((newX - m_track.getPosition().x) / m_track.getSize().x) * 100.f;
	
		m_fill.setSize({ newX - m_track.getPosition().x, m_track.getSize().y });
	}
	return m_value;
}

void Slider::setPosition(sf::Vector2f pos) 
{
	m_track.setPosition(pos);
	m_fill.setPosition(pos);

	float currentWidth = (m_value / 100.f) * m_track.getSize().x;
	m_handle.setPosition({ pos.x + currentWidth, pos.y });
	m_fill.setSize({ currentWidth, m_track.getSize().y });
}

void Slider::draw(sf::RenderWindow& window) const
{
	window.draw(m_track);
	window.draw(m_fill);
	window.draw(m_handle);
}