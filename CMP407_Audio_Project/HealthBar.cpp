#include "HealthBar.h"

HealthBar::HealthBar(float maxHealth)
{
	_maxHealth = maxHealth;
	_currentHealth = maxHealth;

	_barSize = { 200.f, 20.f };

	_backBar.setSize(_barSize);
	_backBar.setFillColor(sf::Color(50, 50, 50)); // Dark Gray
	_backBar.setOutlineThickness(2.f);
	_backBar.setFillColor(sf::Color::White);

	_frontBar.setSize(_barSize);
	_frontBar.setFillColor(sf::Color::Green);

	_backBar.setPosition({ 20.f, 20.f });
	_frontBar.setPosition({ 20.f, 20.f });
}

void HealthBar::Update(float currentHealth) 
{
	_currentHealth = currentHealth;

	// Prevent health from going below 0 or above the max health
	if (_currentHealth < 0) _currentHealth = 0;
	if (_currentHealth > _maxHealth) _currentHealth = _maxHealth;

	float healthPercent = _currentHealth / _maxHealth;

	_frontBar.setSize({ _barSize.x * healthPercent, _barSize.y });

	if (healthPercent < 0.25f) _frontBar.setFillColor(sf::Color::Red);
	else if (healthPercent < 0.5f) _frontBar.setFillColor(sf::Color::Yellow);
	else _frontBar.setFillColor(sf::Color::Green);
}

void HealthBar::Render(sf::RenderWindow* window) 
{
	window->draw(_backBar);
	window->draw(_frontBar);
}
