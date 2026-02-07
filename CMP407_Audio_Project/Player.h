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

	float GetHealth() const { return _health; }
	void SetHealth(int health) { _health = health; }

	float GetRotation() const { return _rotation; }
	void SetRotation(float rotation) { _rotation = rotation; }

	sf::Vector2f GetPosition() const { return _playerShape.getPosition(); }
	void SetPosition(sf::Vector2f position) { _playerShape.setPosition(position); }

	void Update(float dt);
	void Render();

private:
	float _health = 100.f;
	float _rotation = 0.f;

	sf::CircleShape _playerShape;
	sf::RectangleShape _shieldShape;
	sf::VertexArray _lightCone;
	sf::RenderWindow* _window;

	void UpdateFlashlightVisuals();

};

