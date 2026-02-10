#pragma once
#include <SFML/Graphics.hpp>
#include <Vector>
#include "Player.h"
#include "Enemy.h"
#include "HealthBar.h"

class DynamicMain
{
public:
	DynamicMain(sf::RenderWindow* window, sf::Font* font);
	~DynamicMain() {}

	sf::Color GetBackgroundColour() const;

	void Update(float dt);
	void Render();

private:
	sf::RenderWindow* _window;
	Player _player;
	std::vector<Enemy> _enemies;
	HealthBar* _healthBar;
	sf::Font* _font;
	sf::Text _debugText;

	float _spawnTimer = 0.f;
	float _totalPlayTime = 0.f; // How long the game has been running for
	float _currentSpawnRate = 3.f;
	const float _START_SPAWN_RATE = 3.f;
	const float MIN_SPAWN_RATE = 0.5f; // Fastest spawn speed (0.5s) 
	const float DIFFICULTY_RAMP = 0.03f; // How much to decrease delay by per second
	int _enemiesDefended = 0;
	float _darknessFactor = 0.f;

	bool _showDebug = false; // Is the text visible
	bool _isPPressed = false; // Prevents constant flickering when the key is pressed

	void UpdateDebugText(float dt);
	bool IsAngleInView(float enemyAngle, float playerAngle, float fieldOfView);

};

