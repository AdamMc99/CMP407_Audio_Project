#include "DynamicMain.h"
#include <cmath>
#include <algorithm>
#include <iostream>

constexpr float PI = 3.14159265f;
constexpr float RAD_TO_DEG = 180.f / PI;
constexpr float DEG_TO_RAD = PI / 180.f;

DynamicMain::DynamicMain(sf::RenderWindow* window, sf::Font* font) : _window(window), _player(window), _font(font), _debugText(*font)
{
	_window->setMouseCursorVisible(true);

	_healthBar = new HealthBar(100.f);
	_darknessFactor = 0.f;

	_debugText.setCharacterSize(14); 
	_debugText.setFillColor(sf::Color::White);
	_debugText.setOutlineColor(sf::Color::Black);
	_debugText.setOutlineThickness(1.f);
}

sf::Color DynamicMain::GetBackgroundColour() const
{
	//sf::Color dayColour = sf::Color(100,100,100); // Gray for "daytime"
	sf::Color dayColour = sf::Color(72,72,56); // Gray for "daytime"
	sf::Color nightColour = sf::Color::Black;	  // Black for "night"

	// Interpolate each value for smooth transition
	float r = dayColour.r + (nightColour.r - dayColour.r) * _darknessFactor;
	float g = dayColour.g + (nightColour.g - dayColour.g) * _darknessFactor;
	float b = dayColour.b + (nightColour.b - dayColour.b) * _darknessFactor;

	return sf::Color(r,g,b);
}

void DynamicMain::Update(float dt)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
	{
		if (!_isPPressed)
		{
			_showDebug = !_showDebug; 
			_isPPressed = true;       
		}
	}
	else _isPPressed = false;

	if (_showDebug) UpdateDebugText(dt);

	_totalPlayTime += dt;

	float newSpawnRate = _START_SPAWN_RATE - (_totalPlayTime * DIFFICULTY_RAMP);
	if (newSpawnRate < MIN_SPAWN_RATE) newSpawnRate = MIN_SPAWN_RATE;
	_currentSpawnRate = newSpawnRate;

	if (_enemiesDefended >= 10 && _darknessFactor < 1.f) 
	{
		_darknessFactor += dt * 0.5f; // Fade to dark over ~2 seconds
		if (_darknessFactor > 1.f) _darknessFactor = 1.0f;
		_window->setMouseCursorVisible(false);
	}

	// Update the player
	_player.Update(dt, _darknessFactor);

	// Spawn enemies
	_spawnTimer += dt;
	if (_spawnTimer >= _currentSpawnRate) 
	{
		_spawnTimer = 0.f;

		// Calculate random spawn position
		float angle = (rand() % 360) * DEG_TO_RAD;
		float dist = 500.f;
		sf::Vector2f playerPos = _player.GetPosition();

		sf::Vector2f spawnPos = { playerPos.x + std::cos(angle) * dist, playerPos.y + std::sin(angle) * dist };

		// Create the enemy
		_enemies.emplace_back(spawnPos, playerPos);
	}


	// Update enemies and collision
	sf::Vector2f playerPos = _player.GetPosition();
	float playerAngle = _player.GetRotation();

	for (auto& enemy : _enemies) 
	{
		if (!enemy.IsActive()) continue;

		enemy.Update(dt);
		enemy.UpdateVisibility(_player, _darknessFactor);

		sf::Vector2f enemyPos = enemy.GetPosition();

		float dx = enemyPos.x - playerPos.x;
		float dy = enemyPos.y - playerPos.y;
		float disSqr = dx * dx + dy * dy;

		float enemyAngle = std::atan2(dy, dx) * RAD_TO_DEG;

		// Check collision with player body
		if (disSqr < 35.f * 35.f) 
		{
			enemy.Destroy();
			_player.TakeDamage(5);
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
				_player.AddHealth(1.f);
				_enemiesDefended++;
			}
		}
	}

	_healthBar->Update(_player.GetHealth());

	// Remove inactive enemies
	_enemies.erase(std::remove_if(_enemies.begin(), _enemies.end(), [](const Enemy& e) {return !e.IsActive(); }), _enemies.end());
}

void DynamicMain::UpdateDebugText(float dt)
{
	std::string info = "";

	int fps = static_cast<int>(1.f / dt);

	info += "FPS: " + std::to_string(fps) + "\n";
	info += "Enemies: " + std::to_string(_enemies.size()) + "\n";
	info += "Enemies Defeated: " + std::to_string(_enemiesDefended) + "\n";
	info += "Darkness: " + std::to_string(_darknessFactor) + "\n";
	info += "Spawn Rate: " + std::to_string(_currentSpawnRate) + "\n";

	_debugText.setString(info);

	// Update origin every frame as text can change
	sf::FloatRect bounds = _debugText.getLocalBounds();
	_debugText.setOrigin({ bounds.size.x, 0 }); // Anchor top right corner of text

	// Position at top right of the window with 10px padding
	_debugText.setPosition({ _window->getSize().x - 10.f, 10.f });
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
		bool isVisibleInFlashlight = IsAngleInView(enemyAngle, playerAngle, VIEW_ANGLE);
		if (_darknessFactor < 0.95f || isVisibleInFlashlight) 
		{
			enemy.Render(_window);
		}
	}

	_healthBar->Render(_window);

	if (_showDebug) _window->draw(_debugText);
}
