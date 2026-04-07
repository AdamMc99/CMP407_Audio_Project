#include "DynamicMain.h"
#include <cmath>
#include <algorithm>
#include <iostream>

constexpr float PI = 3.14159265f;
constexpr float RAD_TO_DEG = 180.f / PI;
constexpr float DEG_TO_RAD = PI / 180.f;

DynamicMain::DynamicMain(sf::RenderWindow* window, sf::Font& font, WwiseWrapper& wwise) 
	: m_window(window), m_player(window), m_font(font), m_debugText(font), m_wwise(wwise)
{
	m_window->setMouseCursorVisible(true);

	m_healthBar = new HealthBar(100.f);
	m_darknessFactor = 0.f;

	m_debugText.setCharacterSize(14); 
	m_debugText.setFillColor(sf::Color::White);
	m_debugText.setOutlineColor(sf::Color::Black);
	m_debugText.setOutlineThickness(1.f);
}

void DynamicMain::reset() 
{
	// Reset variables
	m_totalPlayTime = 0.f;
	m_spawnTimer = 0.f;
	m_currentSpawnRate = START_SPAWN_RATE;
	m_darknessFactor = 0.f;
	m_enemiesDefended = 0;
	m_isPPressed = false;
	m_showDebug = false;

	// Clear entities
	m_enemies.clear();

	// Reset sub-components
	m_player.reset();

	// Reset UI
	m_healthBar->update(m_player.getHealth());

	// Reset window changes
	m_window->setMouseCursorVisible(true);
}

sf::Color DynamicMain::getBackgroundColour() const
{
	//sf::Color dayColour = sf::Color(100,100,100); // Gray for "daytime"
	sf::Color dayColour = sf::Color(72,72,56); // Gray for "daytime"
	sf::Color nightColour = sf::Color::Black;	  // Black for "night"

	// Interpolate each value for smooth transition
	float r = dayColour.r + (nightColour.r - dayColour.r) * m_darknessFactor;
	float g = dayColour.g + (nightColour.g - dayColour.g) * m_darknessFactor;
	float b = dayColour.b + (nightColour.b - dayColour.b) * m_darknessFactor;

	return sf::Color(r,g,b);
}

void DynamicMain::update(float dt)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
	{
		if (!m_isPPressed)
		{
			m_showDebug = !m_showDebug; 
			m_isPPressed = true;       
		}
	}
	else m_isPPressed = false;

	if (m_showDebug) updateDebugText(dt);

	m_totalPlayTime += dt;

	float newSpawnRate = START_SPAWN_RATE - (m_totalPlayTime * DIFFICULTY_RAMP);
	if (newSpawnRate < MIN_SPAWN_RATE) newSpawnRate = MIN_SPAWN_RATE;
	m_currentSpawnRate = newSpawnRate;

	if (m_enemiesDefended >= 6 && m_darknessFactor < 1.f) 
	{
		m_darknessFactor += dt * 0.5f; // Fade to dark over ~2 seconds
		if (m_darknessFactor > 1.f) m_darknessFactor = 1.0f;
		m_window->setMouseCursorVisible(false);
	}

	// Update the player
	m_player.update(dt, m_darknessFactor);

	// Spawn enemies
	m_spawnTimer += dt;
	if (m_spawnTimer >= m_currentSpawnRate) 
	{
		m_spawnTimer = 0.f;

		// Calculate random spawn position
		float angle = (rand() % 360) * DEG_TO_RAD;
		float dist = 500.f;
		sf::Vector2f playerPos = m_player.getPosition();

		sf::Vector2f spawnPos = { playerPos.x + std::cos(angle) * dist, playerPos.y + std::sin(angle) * dist };

		// Create the enemy
		m_enemies.emplace_back(spawnPos, playerPos);
	}


	// Update enemies and collision
	sf::Vector2f playerPos = m_player.getPosition();
	float playerAngle = m_player.getRotation();

	for (auto& enemy : m_enemies) 
	{
		if (!enemy.isActive()) continue;

		enemy.update(dt);
		enemy.updateVisibility(m_player, m_darknessFactor);

		sf::Vector2f enemyPos = enemy.getPosition();

		float dx = enemyPos.x - playerPos.x;
		float dy = enemyPos.y - playerPos.y;
		float disSqr = dx * dx + dy * dy;

		float enemyAngle = std::atan2(dy, dx) * RAD_TO_DEG;

		// Check collision with player body
		if (disSqr < 35.f * 35.f) 
		{
			enemy.destroy();
			m_player.takeDamage(5);
		}

		// Check collision wih shield
		float currentPos = std::sqrt(disSqr);
		// Shield range is roughly 50 to 80 units away
		if (currentPos > 50.f && currentPos < 80.f) 
		{
			// Check if enemy is within shields arc (25 degree tolerance)
			if (isAngleInView(enemyAngle, playerAngle, 25.f)) 
			{
				enemy.destroy();
				//m_player.addHealth(1.f); // Adds health to the player if they block an enemy
				m_enemiesDefended++;
			}
		}
	}

	// Update intensity value
	float enemyIntensity = (static_cast<float>(m_enemies.size()) / 15) * 100.f;
	if (enemyIntensity > 100.f) enemyIntensity = 100.f;

	float healthPanic = 0.f;
	if (m_player.getHealth() < 50.f) 
	{
		// 50 health = 0 panic, 0 health = 100 panic
		healthPanic = ((50.f - m_player.getHealth()) / 50.f) * 100.f;
	}

	m_intensity = std::max(enemyIntensity, healthPanic);
	m_wwise.setRTPCValue("Game_Intensity", m_intensity, m_gameAudioID);

	// Update UI
	m_healthBar->update(m_player.getHealth());

	// Remove inactive enemies
	m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(), [](const Enemy& e) {return !e.isActive(); }), m_enemies.end());
}

void DynamicMain::updateDebugText(float dt)
{
	std::string info = "";

	int fps = static_cast<int>(1.f / dt);

	info += "FPS: " + std::to_string(fps) + "\n";
	info += "Enemies: " + std::to_string(m_enemies.size()) + "\n";
	info += "Enemies Defeated: " + std::to_string(m_enemiesDefended) + "\n";
	info += "Darkness: " + std::to_string(m_darknessFactor) + "\n";
	info += "Spawn Rate: " + std::to_string(m_currentSpawnRate) + "\n";
	info += "Intensity: " + std::to_string(m_intensity) + "\n";

	m_debugText.setString(info);

	// Update origin every frame as text can change
	sf::FloatRect bounds = m_debugText.getLocalBounds();
	m_debugText.setOrigin({ bounds.size.x, 0 }); // Anchor top right corner of text

	// Position at top right of the window with 10px padding
	m_debugText.setPosition({ m_window->getSize().x - 10.f, 10.f });
}

bool DynamicMain::isAngleInView(float enemyAngle, float playerAngle, float fov)
{
	float diff = enemyAngle - playerAngle;
	while (diff < -180.f) diff += 360.f;
	while (diff > 180.f) diff -= 360.f;
	return std::abs(diff) <= fov;
}

void DynamicMain::render() 
{
	m_player.render();

	sf::Vector2f playerPos = m_player.getPosition();
	float playerAngle = m_player.getRotation();

	for (auto& enemy : m_enemies) 
	{
		if (!enemy.isActive()) continue;

		// Recalculate angle here to decide if enemy is drawn
		sf::Vector2f enemyPos = enemy.getPosition();
		float dx = enemyPos.x - playerPos.x;
		float dy = enemyPos.y - playerPos.y;
		float enemyAngle = std::atan2(dy, dx) * RAD_TO_DEG;

		// Only render enemy if its within the light cone
		bool isVisibleInFlashlight = isAngleInView(enemyAngle, playerAngle, VIEW_ANGLE);
		if (m_darknessFactor < 0.95f || isVisibleInFlashlight) 
		{
			enemy.render(m_window);
		}
	}

	m_healthBar->render(m_window);

	if (m_showDebug) m_window->draw(m_debugText);
}

void DynamicMain::playAudio() 
{
	m_wwise.registerGameObject(m_gameAudioID, "Game Audio");
	m_wwise.postEvent("Play_Game_BGM", m_gameAudioID);
	//m_wwise.postEvent("Loop", m_gameAudioID);
}

void DynamicMain::stopAudio() 
{
	m_wwise.stopAll(m_gameAudioID);
	m_wwise.unregisterGameObject(m_gameAudioID);
}
