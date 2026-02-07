#pragma once
#include <SFML/Graphics.hpp>
#include <Vector>
#include "Player.h"
#include "Enemy.h"

class DynamicMain
{
public:
	DynamicMain(sf::RenderWindow* window);
	~DynamicMain() {}

	void Update(float dt);
	void Render();

private:
	sf::RenderWindow* _window;
	Player _player;
	std::vector<Enemy> _enemies;

	float _spawnTimer = 0.f;
	float _spawnRate = 1.f;

	bool IsAngleInView(float enemyAngle, float playerAngle, float fieldOfView);

};

