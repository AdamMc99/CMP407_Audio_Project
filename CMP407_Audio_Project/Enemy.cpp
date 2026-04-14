#include "Enemy.h"
#include <cmath>

Enemy::Enemy(sf::Vector2f spawnPos, sf::Vector2f targetPos) : m_active(true)
{
	m_enemyShape.setRadius(15.f);
	m_enemyShape.setOrigin({ 15.f, 15.f });
	m_enemyShape.setFillColor(sf::Color::Red);
	m_enemyShape.setPosition(spawnPos);

	// Calculate direction towards player
	sf::Vector2f direction = targetPos - spawnPos;
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

	// Normalise and multiply by speed
	if (length != 0) 
	{
		m_velocity = (direction / length) * m_speed;
	}
	else 
	{
		m_velocity = { 0.f, 0.f };
	}
}

void Enemy::updateVisibility(const Player& player, float darknessFactor)
{
    sf::Vector2f playerPos = player.getPosition();
    sf::Vector2f enemyPos = m_enemyShape.getPosition();

    float dx = enemyPos.x - playerPos.x;
    float dy = enemyPos.y - playerPos.y;

    float distance = std::sqrt(dx * dx + dy * dy);
    float lightRange = player.getLightRange();
    float flashlightAlpha = 0.f;
    float ambientAlpha = 0.f;

    float ambientRadius = 80.f;
    if (distance <= ambientRadius) 
    {
        ambientAlpha = (1.f - (distance / ambientRadius));
    }


    if (distance <= lightRange) 
    {
        float enemyAngleRad = std::atan2(dy, dx);
        float enemyAngleDeg = enemyAngleRad * (180 / 3.14159265);
        float playerAngleDeg = player.getRotation();

        // Get the absolute difference between angles
        float angleDiff = std::abs(playerAngleDeg - enemyAngleDeg);

        if (angleDiff > 180.f) {
            angleDiff = 360.f - angleDiff;
        }

        float viewAngle = player.getFOV();

        float angularFadeInfo = 15.f;
        float angleFactor = (viewAngle - angleDiff) / angularFadeInfo;

        float distFadeInfo = 50.f;
        float distFactor = (lightRange - distance) / distFadeInfo;

        float totalAlphaFactor = std::min(angleFactor, distFactor);
        totalAlphaFactor = std::max(0.0f, std::min(totalAlphaFactor, 1.0f));

        float finalAlphaFactor = std::max(ambientAlpha, totalAlphaFactor);
        flashlightAlpha = 255 * finalAlphaFactor;
    }
    else if (distance <= ambientRadius) 
    {
        // If not in torch range but ARE in ambient range
        flashlightAlpha = 255 * ambientAlpha;
    }

    float dayLightAlpha = 255.f;
    float finalAlpha = (flashlightAlpha * darknessFactor) + (dayLightAlpha * (1.f - darknessFactor));

    sf::Color colour = m_enemyShape.getFillColor();
    colour.a = finalAlpha;
    m_enemyShape.setFillColor(colour);
}

void Enemy:: update(float dt)
{
	m_enemyShape.move(m_velocity * dt);
}

void Enemy::render(sf::RenderWindow* window) 
{
	window->draw(m_enemyShape);
}


