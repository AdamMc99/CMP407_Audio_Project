#include "Player.h"

constexpr float PI = 3.14159265f;
constexpr float DEG_TO_RAD = PI / 180.f;
constexpr float RAD_TO_DEG = 180.f / PI;

Player::Player(sf::RenderWindow* window) : m_window(window)
{
	// Setup player body
	m_playerShape.setRadius(20.f);
	m_playerShape.setOrigin({ 20.f, 20.f });
	m_playerShape.setFillColor(sf::Color::Cyan);
	m_playerShape.setPosition(sf::Vector2f(window->getSize()) / 2.f);

	// Setup shield visuals
	m_shieldShape.setSize({ 10.f, SHIELD_SIZE });
	m_shieldShape.setOrigin({ 5.f, SHIELD_SIZE / 2 });
	m_shieldShape.setFillColor(sf::Color::Yellow);

	// Setup light cone
	m_lightCone.setPrimitiveType(sf::PrimitiveType::TriangleFan);
}

void Player::reset()
{
	m_currentHealth = m_maxHealth;
	m_playerShape.setPosition({500.f, 500.f});
	m_playerShape.setRotation(sf::degrees(0));
}

void Player::update(float dt, float darknessFactor, sf::Vector2f mouseWorldPos)
{
	sf::Vector2f playerPos = m_playerShape.getPosition();

	// Calculate angle using the true world coordinates of the mouse
	float dx = mouseWorldPos.x - playerPos.x;
	float dy = mouseWorldPos.y - playerPos.y;

	float rotationRad = std::atan2(dy, dx);
	m_rotation = rotationRad * RAD_TO_DEG;

	// Position the shield around the player
	float shieldX = playerPos.x + std::cos(rotationRad) * SHIELD_DISTANCE;
	float shieldY = playerPos.y + std::sin(rotationRad) * SHIELD_DISTANCE;

	m_shieldShape.setPosition({ shieldX, shieldY });
	m_shieldShape.setRotation(sf::degrees(m_rotation));

	updateFlashlightVisuals(darknessFactor);
}

void Player::updateFlashlightVisuals(float darknessFactor) 
{
	m_lightCone.clear();

	// If its bright (darkness is almost 0) dont generate cone
	if (darknessFactor <= 0.01f) return;

	sf::Vector2f center = m_playerShape.getPosition();

	// This should fade the light in. Max alpha is 100. Current alpha is 100 * darknessFactor
	std::uint8_t lightAlpha = static_cast<std::uint8_t>(100 * darknessFactor);

	// Set the center point of the light to yellow
	m_lightCone.append({ center, sf::Color(255, 255, 200, lightAlpha), {} }); // <-- This also might not work (It does)
	//m_lightCone.append({ center, sf::Color(255, 100, 100, lightAlpha), {} }); // Red hue. Could maybe adjust colour around player when they chnage health?

	// Create the arc
	float startAngle = (m_rotation - VIEW_ANGLE) * DEG_TO_RAD;
	float endAngle = (m_rotation + VIEW_ANGLE) * DEG_TO_RAD;
	int segments = 20;

	for(int i = 0; i <= segments; i++)
	{
		float t = static_cast<float>(i) / segments;
		float currentAngle = startAngle + t * (endAngle - startAngle);

		float x = center.x + std::cos(currentAngle) * m_lightRange;
		float y = center.y + std::sin(currentAngle) * m_lightRange;

		// Outer edges fade to transparent
		m_lightCone.append({ { x,y }, sf::Color(255, 255, 200, 0), {} });
	}
}

void Player::render() 
{
	m_window->draw(m_lightCone);
	m_window->draw(m_playerShape);
	m_window->draw(m_shieldShape);
}



