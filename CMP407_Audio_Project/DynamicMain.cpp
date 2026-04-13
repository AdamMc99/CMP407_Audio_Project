#include "DynamicMain.h"
#include <cmath>        
#include <algorithm>   
#include <iostream>

// Constants for angle conversions
constexpr float PI = 3.14159265f;
constexpr float RAD_TO_DEG = 180.f / PI;
constexpr float DEG_TO_RAD = PI / 180.f;

// Set up main game system
DynamicMain::DynamicMain(sf::RenderWindow* window, sf::Font& font, WwiseWrapper& wwise)
	: m_window(window), m_player(window), m_font(font), m_wwise(wwise)
{
	// Show the mouse cursor
	m_window->setMouseCursorVisible(true);

	// Create a health bar with max value 100
	m_healthBar = new HealthBar(100.f);

	// No darkness at the start
	m_darknessFactor = 0.f;

	// Load background texture
	if (!m_backgroundTexture.loadFromFile("Assets/Textures/stoneBackground.png"))
	{
		std::cerr << "ERROR: Could not load background texture - DynamicMain.cpp \n";
	}

	// Let texture repeat
	m_backgroundTexture.setRepeated(true);
	m_backgroundTexture.setSmooth(false);

	// Create a large background area
	constexpr int WORLD_HALF = 4000;
	m_backgroundSprite.setTextureRect(sf::IntRect({ -WORLD_HALF, -WORLD_HALF }, { WORLD_HALF * 2, WORLD_HALF * 2 }));
	m_backgroundSprite.setPosition({ -WORLD_HALF, -WORLD_HALF });

	// Create a render texture for darkness overlay
	auto size = m_window->getSize();
	if (!m_darknessOverlay.resize({ size.x, size.y }))
	{
		// Handle error if needed
	}
}


// Reset the game state
void DynamicMain::reset()
{
	// Reset gameplay values
	m_totalPlayTime = 0.f;
	m_spawnTimer = 0.f;
	m_currentSpawnRate = START_SPAWN_RATE;
	m_darknessFactor = 0.f;
	m_enemiesDefended = 0;
	m_intensity = 0.f;
	m_calculatedIntensity = 0.f;

	// Remove all enemies
	m_enemies.clear();

	// Reset player state
	m_player.reset();

	// Update UI
	m_healthBar->update(m_player.getHealth());

	// Show cursor again
	m_window->setMouseCursorVisible(true);

	// Reset debug values
	m_debugIntesityModifier = 0.f;
	m_debugPauseIntensity = false;
}


// Returns a colour that changes based on darknessFactor
sf::Color DynamicMain::getBackgroundColour() const
{
	sf::Color dayColour = sf::Color(72, 72, 56);
	sf::Color nightColour = sf::Color::Black;

	// Linearly interpolate between day and night colours
	float r = dayColour.r + (nightColour.r - dayColour.r) * m_darknessFactor;
	float g = dayColour.g + (nightColour.g - dayColour.g) * m_darknessFactor;
	float b = dayColour.b + (nightColour.b - dayColour.b) * m_darknessFactor;

	return sf::Color(r, g, b);
}


// Main update function (called every frame)
void DynamicMain::update(float dt)
{
	// Increase total play time
	m_totalPlayTime += dt;

	// Gradually increase difficulty by reducing spawn time
	float newSpawnRate = START_SPAWN_RATE - (m_totalPlayTime * DIFFICULTY_RAMP);
	if (newSpawnRate < MIN_SPAWN_RATE) newSpawnRate = MIN_SPAWN_RATE;
	m_currentSpawnRate = newSpawnRate;

	// Increase darkness after defending enough enemies
	if (m_enemiesDefended >= 6 && m_darknessFactor < 1.f)
	{
		m_darknessFactor += dt * 0.5f;
		if (m_darknessFactor > 1.f) m_darknessFactor = 1.0f;

		// Hide cursor in dark mode
		m_window->setMouseCursorVisible(false);
	}

	// Update player
	sf::View worldView(m_player.getPosition(), { static_cast<float>(m_window->getSize().x), static_cast<float>(m_window->getSize().y) });

	// Get mouse position in world coordinates
	sf::Vector2i pixelPos = sf::Mouse::getPosition(*m_window);
	sf::Vector2f worldMousePos = m_window->mapPixelToCoords(pixelPos, worldView);

	m_player.update(dt, m_darknessFactor, worldMousePos);

	// Spawn enemies over time
	m_spawnTimer += dt;
	if (m_spawnTimer >= m_currentSpawnRate)
	{
		m_spawnTimer = 0.f;

		// Random direction
		float angle = (rand() % 360) * DEG_TO_RAD;

		// Calculate distance to screen corner
		float winWidth = static_cast<float>(m_window->getSize().x);
		float winHeight = static_cast<float>(m_window->getSize().y);

		float cornerDistance = std::sqrt((winWidth / 2.f) * (winWidth / 2.f) + (winHeight / 2.f) * (winHeight / 2.f));

		// Spawn outside screen
		float dist = cornerDistance + 50.f;

		sf::Vector2f playerPos = m_player.getPosition();

		// Calculate spawn position using angle
		sf::Vector2f spawnPos = { playerPos.x + std::cos(angle) * dist, playerPos.y + std::sin(angle) * dist };

		m_enemies.emplace_back(spawnPos, playerPos);
	}


	// Update enemies and check collisions
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

		// Collision with player body
		if (disSqr < 35.f * 35.f)
		{
			enemy.destroy();
			m_player.takeDamage(5);
		}

		// Collision with shield
		float currentPos = std::sqrt(disSqr);

		if (currentPos > 50.f && currentPos < 80.f)
		{
			if (isAngleInView(enemyAngle, playerAngle, 25.f))
			{
				enemy.destroy();
				m_enemiesDefended++;
			}
		}
	}

	// Calculate intensity (used for audio)
	if (!m_debugPauseIntensity)
	{
		float enemyIntensity = (static_cast<float>(m_enemies.size()) / 15) * 100.f;
		if (enemyIntensity > 100.f) enemyIntensity = 100.f;

		float healthPanic = 0.f;

		if (m_player.getHealth() < 50.f)
		{
			healthPanic = ((50.f - m_player.getHealth()) / 50.f) * 100.f;
		}

		m_calculatedIntensity = std::max(enemyIntensity, healthPanic);
	}

	float addedIntensity = m_debugIntesityModifier * 10;
	m_intensity = m_calculatedIntensity + addedIntensity;

	// Send intensity value to audio system
	m_wwise.setRTPCValue("Game_Intensity", m_intensity, m_gameAudioID);

	// Update UI
	m_healthBar->update(m_player.getHealth());

	// Remove inactive enemies from vector
	m_enemies.erase(
		std::remove_if(m_enemies.begin(), m_enemies.end(),
			[](const Enemy& e) { return !e.isActive(); }),
		m_enemies.end()
	);
}


// Checks if an angle is within a given field of view
bool DynamicMain::isAngleInView(float enemyAngle, float playerAngle, float fov)
{
	float diff = enemyAngle - playerAngle;

	// Keep angle difference between -180 and 180
	while (diff < -180.f) diff += 360.f;
	while (diff > 180.f) diff -= 360.f;

	return std::abs(diff) <= fov;
}

void DynamicMain::render()
{
	handleResize();

	const float winW = static_cast<float>(m_window->getSize().x);
	const float winH = static_cast<float>(m_window->getSize().y);

	sf::View worldView(m_player.getPosition(), { winW, winH });
	m_window->setView(worldView);

	m_backgroundSprite.setColor(sf::Color::White);
	m_window->draw(m_backgroundSprite);

	sf::Vector2f playerPos = m_player.getPosition();

	float playerAngle = m_player.getRotation();

	for (auto& enemy : m_enemies)
	{
		if (!enemy.isActive()) continue;

		sf::Vector2f enemyPos = enemy.getPosition();
		float dx = enemyPos.x - playerPos.x;
		float dy = enemyPos.y - playerPos.y;
		float enemyAngle = std::atan2(dy, dx) * RAD_TO_DEG;

		bool inTorch = isAngleInView(enemyAngle, playerAngle, VIEW_ANGLE);

		if (m_darknessFactor < 0.95f || inTorch)
			enemy.render(m_window);
	}

	sf::View uiView(sf::FloatRect({ 0.f, 0.f }, { winW, winH }));
	m_window->setView(uiView);

	if (m_darknessFactor > 0.01f) 
	{
		std::uint8_t darkAlpha = static_cast<std::uint8_t>(m_darknessFactor * 255);

		m_darknessOverlay.clear(sf::Color(0, 0, 0, darkAlpha));

		sf::BlendMode cutout(
			sf::BlendMode::Factor::Zero,	// srcColour
			sf::BlendMode::Factor::One,		// dstColour
			sf::BlendMode::Equation::Add,
			sf::BlendMode::Factor::One,		// srcAlpha
			sf::BlendMode::Factor::One,		// dstAlpha
			sf::BlendMode::Equation::ReverseSubtract
		);

		sf::Vector2f screenCentre = { winW / 2, winH / 2 };

		sf::VertexArray torchCone = buildScreenTorch(screenCentre, playerAngle, m_player.getLightRange(), VIEW_ANGLE, m_darknessFactor);
		sf::RenderStates renderState;
		renderState.blendMode = cutout;
		m_darknessOverlay.draw(torchCone, renderState);
		m_darknessOverlay.display();

		sf::Sprite darkSrpite(m_darknessOverlay.getTexture());
		m_window->draw(darkSrpite);
	}

	m_healthBar->render(m_window);

	m_window->setView(worldView);
	m_player.render();

}

void DynamicMain::handleResize() 
{
	auto size = m_window->getSize();
	if (size == m_darknessOverlay.getSize()) return;

	m_darknessOverlay.resize({ size.x, size.y });
}

sf::VertexArray DynamicMain::buildScreenTorch(sf::Vector2f center, float angleDeg, float range, float halfFOV, float darknessFactor) const
{
	sf::VertexArray cone(sf::PrimitiveType::Triangles);

	// Change the values to shape the falloff curve
	struct Ring { float radiusFraction; float alphaFraction; };
	constexpr Ring rings[] = {
		{ 0.00f, 1.00f },  // centre fully cuts darkness
		{ 0.35f, 0.95f },  // still very bright
		{ 0.60f, 0.70f },  // beginning to fade
		{ 0.80f, 0.30f },  // dimmer
		{ 1.00f, 0.00f },  // outer edge no cutout
	};
	constexpr int RING_COUNT = sizeof(rings) / sizeof(rings[0]);

	std::uint8_t maxAlpha = static_cast<std::uint8_t>(255 * darknessFactor);

	float startRad = (angleDeg - halfFOV) * DEG_TO_RAD;
	float endRad = (angleDeg + halfFOV) * DEG_TO_RAD;
	constexpr int SEGMENTS = 24;

	for (int r = 0; r < RING_COUNT - 1; ++r)
	{
		float innerR = rings[r].radiusFraction * range;
		float outerR = rings[r + 1].radiusFraction * range;

		auto innerAlpha = static_cast<std::uint8_t>(maxAlpha * rings[r].alphaFraction);
		auto outerAlpha = static_cast<std::uint8_t>(maxAlpha * rings[r + 1].alphaFraction);

		sf::Color innerCol(0, 0, 0, innerAlpha);
		sf::Color outerCol(0, 0, 0, outerAlpha);

		for (int i = 0; i < SEGMENTS; ++i)
		{
			float edge0 = static_cast<float>(i) / SEGMENTS;
			float edge1 = static_cast<float>(i + 1) / SEGMENTS;
			float angle0 = startRad + edge0 * (endRad - startRad);
			float angle1 = startRad + edge1 * (endRad - startRad);

			sf::Vector2f inner0 = { center.x + std::cos(angle0) * innerR, center.y + std::sin(angle0) * innerR };
			sf::Vector2f inner1 = { center.x + std::cos(angle1) * innerR, center.y + std::sin(angle1) * innerR };
			sf::Vector2f outer0 = { center.x + std::cos(angle0) * outerR, center.y + std::sin(angle0) * outerR };
			sf::Vector2f outer1 = { center.x + std::cos(angle1) * outerR, center.y + std::sin(angle1) * outerR };

			// Two triangles forming a quad between the two rings
			cone.append({ inner0, innerCol });
			cone.append({ inner1, innerCol });
			cone.append({ outer0, outerCol });

			cone.append({ inner1, innerCol });
			cone.append({ outer1, outerCol });
			cone.append({ outer0, outerCol });
		}
	}

	return cone;
}

void DynamicMain::playAudio() 
{
	m_wwise.registerGameObject(m_gameAudioID, "Game BGM Audio");
	m_wwise.postEvent("Play_Game_BGM", m_gameAudioID);
}

void DynamicMain::stopAudio() 
{
	m_wwise.stopAll();
	m_wwise.unregisterGameObject(m_gameAudioID);
}