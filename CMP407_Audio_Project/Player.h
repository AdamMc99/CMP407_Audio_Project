#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

const float SHIELD_DISTANCE = 40.f;
const float SHIELD_SIZE = 40.f;
const float VIEW_ANGLE = 45.f;

class Player
{
public:
	Player(sf::RenderWindow* window);
	~Player() {}

	float getHealth() const { return m_currentHealth; }
	void setHealth(int health) { m_currentHealth = health; }

	float getRotation() const { return m_rotation; }
	void setRotation(float rotation) { m_rotation = rotation; }

	sf::Vector2f getPosition() const { return m_playerShape.getPosition(); }
	void setPosition(sf::Vector2f position) { m_playerShape.setPosition(position); }

	float getLightRange() const { return m_lightRange; }
	float getFOV() const { return VIEW_ANGLE; }

	void takeDamage(float damage) { m_currentHealth -= damage; }
	void addHealth(float addedHealth) { m_currentHealth += addedHealth; if (m_currentHealth > m_maxHealth) m_currentHealth = m_maxHealth; }

	void update(float dt, float darknessFactor);
	void render();

private:
	float m_maxHealth = 100.f;
	float m_currentHealth = 100.f;
	float m_rotation = 0.f;
	float m_lightRange = 400.f;

	sf::CircleShape m_playerShape;
	sf::RectangleShape m_shieldShape;
	sf::VertexArray m_lightCone;
	sf::RenderWindow* m_window;

	void updateFlashlightVisuals(float darknessFactor);

};

