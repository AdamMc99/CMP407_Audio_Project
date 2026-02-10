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

	float GetHealth() const { return _currentHealth; }
	void SetHealth(int health) { _currentHealth = health; }

	float GetRotation() const { return _rotation; }
	void SetRotation(float rotation) { _rotation = rotation; }

	sf::Vector2f GetPosition() const { return _playerShape.getPosition(); }
	void SetPosition(sf::Vector2f position) { _playerShape.setPosition(position); }

	float GetLightRange() const { return _lightRange; }
	float GetFOV() const { return VIEW_ANGLE; }

	void TakeDamage(float damage) { _currentHealth -= damage; }
	void AddHealth(float addedHealth) { _currentHealth += addedHealth; }

	void Update(float dt, float darknessFactor);
	void Render();

private:
	float _maxHealth = 100.f;
	float _currentHealth = 100.f;
	float _rotation = 0.f;
	float _lightRange = 400.f;

	sf::CircleShape _playerShape;
	sf::RectangleShape _shieldShape;
	sf::VertexArray _lightCone;
	sf::RenderWindow* _window;

	void UpdateFlashlightVisuals(float darknessFactor);

};

