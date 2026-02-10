#include "Enemy.h"
#include <cmath>

Enemy::Enemy(sf::Vector2f spawnPos, sf::Vector2f targetPos) : _active(true)
{
	_enemyShape.setRadius(15.f);
	_enemyShape.setOrigin({ 15.f, 15.f });
	_enemyShape.setFillColor(sf::Color::Red);
	_enemyShape.setPosition(spawnPos);

	// Calculate direction towards player
	sf::Vector2f direction = targetPos - spawnPos;
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

	// Normalise and multiply by speed
	if (length != 0) 
	{
		_velocity = (direction / length) * _speed;
	}
	else 
	{
		_velocity = { 0.f, 0.f };
	}
}

void Enemy::UpdateVisibility(const Player& player, float darknessFactor)
{
    sf::Vector2f playerPos = player.GetPosition();
    sf::Vector2f enemyPos = _enemyShape.getPosition();

    float dx = enemyPos.x - playerPos.x;
    float dy = enemyPos.y - playerPos.y;

    float distance = std::sqrt(dx * dx + dy * dy);
    float lightRange = player.GetLightRange();
    float flashlightAlpha = 0.f;


    if (distance <= lightRange) 
    {
        float enemyAngleRad = std::atan2(dy, dx);
        float enemyAngleDeg = enemyAngleRad * (180 / 3.14159265);
        float playerAngleDeg = player.GetRotation();

        // Get the absolute difference between angles
        float angleDiff = std::abs(playerAngleDeg - enemyAngleDeg);

        if (angleDiff > 180.f) {
            angleDiff = 360.f - angleDiff;
        }

        float viewAngle = player.GetFOV();

        float angularFadeInfo = 15.f;
        float angleFactor = (viewAngle - angleDiff) / angularFadeInfo;

        float distFadeInfo = 50.f;
        float distFactor = (lightRange - distance) / distFadeInfo;

        float totalAlphaFactor = std::min(angleFactor, distFactor);
        totalAlphaFactor = std::max(0.0f, std::min(totalAlphaFactor, 1.0f));

        flashlightAlpha = 255 * totalAlphaFactor;
    }

    float dayLightAlpha = 255.f;
    float finalAlpha = (flashlightAlpha * darknessFactor) + (dayLightAlpha * (1.f - darknessFactor));

    sf::Color colour = _enemyShape.getFillColor();
    colour.a = finalAlpha;
    _enemyShape.setFillColor(colour);
}

void Enemy:: Update(float dt)
{
	_enemyShape.move(_velocity * dt);
}

void Enemy::Render(sf::RenderWindow* window) 
{
	window->draw(_enemyShape);
}


