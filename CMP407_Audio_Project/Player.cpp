#include "Player.h"

constexpr float PI = 3.14159265f;
constexpr float DEG_TO_RAD = PI / 180.f;
constexpr float RAD_TO_DEG = 180.f / PI;

Player::Player(sf::RenderWindow* window) : _window(window)
{
	// Setup player body
	_playerShape.setRadius(20.f);
	_playerShape.setOrigin({ 20.f, 20.f });
	_playerShape.setFillColor(sf::Color::Cyan);
	_playerShape.setPosition(sf::Vector2f(window->getSize()) / 2.f);

	// Setup shield visuals
	_shieldShape.setSize({ 10.f, SHIELD_SIZE });
	_shieldShape.setOrigin({ 5.f, SHIELD_SIZE / 2 });
	_shieldShape.setFillColor(sf::Color::White);

	// Setup light cone
	_lightCone.setPrimitiveType(sf::PrimitiveType::TriangleFan);
}

void Player::Update(float dt) 
{
	// Calculate angle to mouse
	sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
	sf::Vector2f playerPos = _playerShape.getPosition();

	float dx = static_cast<float>(mousePos.x) - playerPos.x;
	float dy = static_cast<float>(mousePos.y) - playerPos.y;

	float rotationRad = std::atan2(dy, dx); // Atan return radians so convert to degrees
	_rotation = rotationRad * RAD_TO_DEG;

	// Position the shield around the player
	float shieldX = playerPos.x + std::cos(rotationRad) * SHIELD_DISTANCE;
	float shieldY = playerPos.y + std::sin(rotationRad) * SHIELD_DISTANCE;

	_shieldShape.setPosition({ shieldX, shieldY });
	_shieldShape.setRotation( sf::degrees(_rotation)); // <--- Using sf::degrees might not work (It does)

	UpdateFlashlightVisuals();
}

void Player::UpdateFlashlightVisuals() 
{
	_lightCone.clear();
	sf::Vector2f center = _playerShape.getPosition();

	// Set the center point of the light to yellow
	_lightCone.append({ center, sf::Color(255, 255, 200, 100), {} }); // <-- This also might not work (It does)

	// Create the arc
	float startAngle = (_rotation - VIEW_ANGLE) * DEG_TO_RAD;
	float endAngle = (_rotation + VIEW_ANGLE) * DEG_TO_RAD;
	int segments = 20;

	for(int i = 0; i <= segments; i++)
	{
		float t = static_cast<float>(i) / segments;
		float currentAngle = startAngle + t * (endAngle - startAngle);

		float x = center.x + std::cos(currentAngle) * _lightRange;
		float y = center.y + std::sin(currentAngle) * _lightRange;

		// Outer edges fade to transparent
		_lightCone.append({ { x,y }, sf::Color(255, 255, 200, 0), {} });
	}
}

void Player::Render() 
{
	_window->draw(_lightCone);
	_window->draw(_playerShape);
	_window->draw(_shieldShape);
}



