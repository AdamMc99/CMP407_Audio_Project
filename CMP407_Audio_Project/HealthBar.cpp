#include "HealthBar.h"

HealthBar::HealthBar(float maxHealth)
{
	m_maxHealth = maxHealth;
	m_currentHealth = maxHealth;

	m_barSize = { 200.f, 20.f };

	m_backBar.setSize(m_barSize);
	m_backBar.setFillColor(sf::Color(50, 50, 50)); // Dark Gray
	m_backBar.setOutlineThickness(2.f);
	m_backBar.setFillColor(sf::Color::White);

	m_frontBar.setSize(m_barSize);
	m_frontBar.setFillColor(sf::Color::Green);

	m_backBar.setPosition({ 20.f, 20.f });
	m_frontBar.setPosition({ 20.f, 20.f });
}

void HealthBar::update(float currentHealth) 
{
	m_currentHealth = currentHealth;

	// Prevent health from going below 0 or above the max health
	if (m_currentHealth < 0) m_currentHealth = 0;
	if (m_currentHealth > m_maxHealth) m_currentHealth = m_maxHealth;

	float healthPercent = m_currentHealth / m_maxHealth;

	m_frontBar.setSize({ m_barSize.x * healthPercent, m_barSize.y });

	if (healthPercent < 0.25f) m_frontBar.setFillColor(sf::Color::Red);
	else if (healthPercent < 0.5f) m_frontBar.setFillColor(sf::Color::Yellow);
	else m_frontBar.setFillColor(sf::Color::Green);
}

void HealthBar::render(sf::RenderWindow* window)
{
	float padding = 20.f;

	// Update the positions dynamically right before drawing
	m_backBar.setPosition({ padding, padding});
	m_frontBar.setPosition({ padding, padding });

	window->draw(m_backBar);
	window->draw(m_frontBar);
}
