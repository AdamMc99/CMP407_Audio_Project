#include "DynamicMain.h"
#include <cmath>
#include <algorithm>
#include <iostream>

constexpr float PI = 3.14159265f;
constexpr float RAD_TO_DEG = 180.f / PI;
constexpr float DEG_TO_RAD = PI / 180.f;

DynamicMain::DynamicMain(sf::RenderWindow* window) : _window(window), _player(window)
{
}

void DynamicMain::Update(float dt)
{
	// Update the player
	_player.Update(dt);
	std::cout << "spawntimer time" << _spawnTimer << std::endl;

	// Spawn enemies
	_spawnTimer += dt;
	if (_spawnTimer >= _spawnRate) 
	{
		_spawnTimer = 0.f;

		// Calculate random spawn position
		float angle = (rand() % 360) * DEG_TO_RAD;
		float dist = 500.f;
		sf::Vector2f playerPos = _player.GetPosition();

		sf::Vector2f spawnPos = { playerPos.x + std::cos(angle) * dist, playerPos.y + std::sin(angle) * dist };

		// Create the enemy
		_enemies.emplace_back(spawnPos, playerPos);
		std::cout << "Enemy spawned at: " << spawnPos.x << ", " << spawnPos.y << " | Total: " << _enemies.size() << std::endl;

	}


	// Update enemies and collision
	sf::Vector2f playerPos = _player.GetPosition();
	float playerAngle = _player.GetRotation();

	for (auto& enemy : _enemies) 
	{
		if (!enemy.IsActive()) continue;

		enemy.Update(dt);

		sf::Vector2f enemyPos = enemy.GetPosition();

		float dx = enemyPos.x - playerPos.x;
		float dy = enemyPos.y - playerPos.y;
		float disSqr = dx * dx + dy * dy;

		float enemyAngle = std::atan2(dy, dx) * RAD_TO_DEG;

		// Check collision with player body
		if (disSqr < 35.f * 35.f) 
		{
			enemy.Destroy();
			_player.SetHealth(_player.GetHealth() - 10);
		}

		// Check collision wih shield
		float currentPos = std::sqrt(disSqr);
		// Shield range is roughly 50 to 80 units away
		if (currentPos > 50.f && currentPos < 80.f) 
		{
			// Check if enemy is within shields arc (25 degree tolerance)
			if (IsAngleInView(enemyAngle, playerAngle, 25.f)) 
			{
				enemy.Destroy();
			}
		}
	}

	// Remove inative enemies
	_enemies.erase(std::remove_if(_enemies.begin(), _enemies.end(), [](const Enemy& e) {return !e.IsActive(); }), _enemies.end());
}

bool DynamicMain::IsAngleInView(float enemyAngle, float playerAngle, float fov) 
{
	float diff = enemyAngle - playerAngle;
	while (diff < -180.f) diff += 360.f;
	while (diff > 180.f) diff -= 360.f;
	return std::abs(diff) <= fov;
}

void DynamicMain::Render()
{
	_player.Render();

	sf::Vector2f playerPos = _player.GetPosition();
	float playerAngle = _player.GetRotation();

	for (auto& enemy : _enemies) 
	{
		if (!enemy.IsActive()) continue;

		// Recalculate angle here to decide if enemy is drawn
		sf::Vector2f enemyPos = enemy.GetPosition();
		float dx = enemyPos.x - playerPos.x;
		float dy = enemyPos.y - playerPos.y;
		float enemyAngle = std::atan2(dy, dx) * RAD_TO_DEG;

		// Only render enemy if its within the light cone
		if (IsAngleInView(enemyAngle, playerAngle, VIEW_ANGLE)) 
		{
			enemy.Render(_window);
		}
	}
}
