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

void Enemy:: Update(float dt)
{
	_enemyShape.move(_velocity * dt);
}

void Enemy::Render(sf::RenderWindow* window) 
{
	window->draw(_enemyShape);
}