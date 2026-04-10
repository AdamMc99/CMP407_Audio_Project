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
	DynamicMain(sf::RenderWindow* window, sf::Font& font, WwiseWrapper& wwise);
	~DynamicMain() {}
	void reset();

	void playAudio();
	void stopAudio();

	sf::Color getBackgroundColour() const;
	bool isCursorHidden() const { return m_darknessFactor >= 1.f; }
	bool isPlayerDead() const { return m_player.getHealth() <= 0.f; }

	void update(float dt);
	void render();

	// Access for DevTools
	size_t getEnemyCount() const { return m_enemies.size(); }
	int getEnemiesDefended() const { return m_enemiesDefended; }
	float getDarknessFactor() const { return m_darknessFactor; }
	float getCurrentSpawnRate() const { return m_currentSpawnRate; }
	float getIntensity() const { return m_intensity; }
	void killAll() { m_enemies.clear(); }
	void addHealth() { m_player.addHealth(10); }
	void removeHealth() { m_player.addHealth(-10); }
	void adjustIntensity(float adjustment) { m_debugIntesityModifier += adjustment; }
	void toggleIntensity() { m_debugPauseIntensity = !m_debugPauseIntensity; }

private:
	sf::RenderWindow* m_window;
	Player m_player;
	std::vector<Enemy> m_enemies;
	HealthBar* m_healthBar;
	sf::Font m_font;
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
	float m_calculatedIntensity = 0.f;
	float m_debugIntesityModifier = 0.f;
	float m_debugPauseIntensity = false;

	bool isAngleInView(float enemyAngle, float playerAngle, float fieldOfView);

};

