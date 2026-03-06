#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
class Enemy
{
public:
	Enemy(sf::Vector2f spawnPos, sf::Vector2f targetPos);
	~Enemy() {}

	void update(float dt);
	// Passing window in here, but main will decide if its rendered based on the lighting
	void render(sf::RenderWindow* window);

	sf::Vector2f getPosition() const { return m_enemyShape.getPosition(); }
	void setPosition(sf::Vector2f position) { m_enemyShape.setPosition(position); }

	void updateVisibility(const Player& player, float darknessFactor);
	
	bool isActive() const { return m_active; }
	void destroy() { m_active = false; }

private:
	sf::CircleShape m_enemyShape;
	sf::Vector2f m_velocity;
	bool m_active;
	float m_speed = 100.f;
};

