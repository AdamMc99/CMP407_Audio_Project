#pragma once
#include <SFML/Graphics.hpp>
#include <Vector>
#include "Player.h"
#include "Enemy.h"
#include "HealthBar.h"
#include "WwiseWrapper.h"

class DynamicMain
{
public:
	DynamicMain(sf::RenderWindow* window, sf::Font* font, WwiseWrapper& wwise);
	~DynamicMain() {}
	void reset();

	void playAudio();
	void stopAudio();

	sf::Color getBackgroundColour() const;
	bool isCursorHidden() const { return m_darknessFactor >= 1.f; }

	void update(float dt);
	void render();

private:
	sf::RenderWindow* m_window;
	Player m_player;
	std::vector<Enemy> m_enemies;
	HealthBar* m_healthBar;
	sf::Font* m_font;
	sf::Text m_debugText;
	WwiseWrapper& m_wwise;
	uint64_t m_gameAudioID = 200;

	const float START_SPAWN_RATE = 3.f; // Longest spawn speed (3.0s)
	const float MIN_SPAWN_RATE = 0.25f; // Fastest spawn speed (0.25s) 
	const float DIFFICULTY_RAMP = 0.03f; // How much to decrease delay by per second

	float m_spawnTimer = 0.f;
	float m_totalPlayTime = 0.f; // How long the game has been running for
	float m_currentSpawnRate = 3.f;
	int m_enemiesDefended = 0;
	float m_darknessFactor = 0.f;
	float m_intensity = 0.f;

	bool m_showDebug = false; // Is the text visible
	bool m_isPPressed = false; // Prevents constant flickering when the key is pressed

	void updateDebugText(float dt);
	bool isAngleInView(float enemyAngle, float playerAngle, float fieldOfView);

};

