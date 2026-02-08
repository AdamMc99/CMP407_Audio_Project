#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
class Enemy
{
public:
	Enemy(sf::Vector2f spawnPos, sf::Vector2f targetPos);
	~Enemy() {}

	void Update(float dt);
	// Passing window in here, but main will decide if its rendered based on the lighting
	void Render(sf::RenderWindow* window);

	sf::Vector2f GetPosition() const { return _enemyShape.getPosition(); }
	void SetPosition(sf::Vector2f position) { _enemyShape.setPosition(position); }

	void UpdateVisibility(const Player& player);
	
	bool IsActive() const { return _active; }
	void Destroy() { _active = false; }

private:
	sf::CircleShape _enemyShape;
	sf::Vector2f _velocity;
	bool _active;
	float _speed = 100.f;
};

