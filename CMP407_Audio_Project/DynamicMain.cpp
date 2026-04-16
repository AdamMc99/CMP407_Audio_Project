#include "DynamicMain.h"
#include <cmath>        
#include <algorithm>   
#include <iostream>

// Constants for angle conversions
constexpr float PI = 3.14159265f;
constexpr float RAD_TO_DEG = 180.f / PI;
constexpr float DEG_TO_RAD = PI / 180.f;
// Constants for gameplay
constexpr int ENEMIES_TO_DEFEAT = 6;

/// <summary>
/// Constructor to intialise main game system, setup background, and reset inital variables.
/// </summary>
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

	// Set background size
	constexpr int WORLD_HALF = 4000;
	m_backgroundSprite.setTextureRect(sf::IntRect({ -WORLD_HALF, -WORLD_HALF }, { WORLD_HALF * 2, WORLD_HALF * 2 }));
	m_backgroundSprite.setPosition({ -WORLD_HALF, -WORLD_HALF });

	// Create a render texture for darkness overlay
	auto size = m_window->getSize();
	if (!m_darknessOverlay.resize({ size.x, size.y }))
	{
		std::cerr << "ERROR: could not create darkness overlay - DynamicMain.cpp \n";
	}
}


/// <summary>
/// Reset the game to its starting conditions for a new run.
/// </summary>
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

/// <summary>
/// Calculates the background colour based on current darkness factor.
/// </summary>
/// <returns>Newly darkened background colour.</returns>
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


/// <summary>
/// Updates player input, enemy spawning, movement, collsions and game difficulty.
/// </summary>
void DynamicMain::update(float dt)
{
	// Increase total play time
	m_totalPlayTime += dt;

	// Gradually increase difficulty by reducing spawn time as game continues
	float newSpawnRate = START_SPAWN_RATE - (m_totalPlayTime * DIFFICULTY_RAMP);
	if (newSpawnRate < MIN_SPAWN_RATE) newSpawnRate = MIN_SPAWN_RATE;
	m_currentSpawnRate = newSpawnRate;

	// Increase darkness after defending enough enemies
	if (m_enemiesDefended >= ENEMIES_TO_DEFEAT && m_darknessFactor < 1.f)
	{
		m_darknessFactor += dt * 0.5f;
		if (m_darknessFactor > 1.f) m_darknessFactor = 1.0f;

		// Hide cursor in dark mode
		m_window->setMouseCursorVisible(false);
	}

	// Set the camera view
	sf::View worldView(m_player.getPosition(), { static_cast<float>(m_window->getSize().x), static_cast<float>(m_window->getSize().y) });

	// Get mouse position in world coordinates
	sf::Vector2i pixelPos = sf::Mouse::getPosition(*m_window);
	sf::Vector2f worldMousePos = m_window->mapPixelToCoords(pixelPos, worldView);

	m_player.update(dt, m_darknessFactor, worldMousePos);

	// Create new enemies if spawn timer reaches target
	m_spawnTimer += dt;
	if (m_spawnTimer >= m_currentSpawnRate)
	{
		m_spawnTimer = 0.f;

		// Random direction from the player
		float angle = (rand() % 360) * DEG_TO_RAD;

		// Calculate spawn location just outside the screen
		float winWidth = static_cast<float>(m_window->getSize().x);
		float winHeight = static_cast<float>(m_window->getSize().y);
		float cornerDistance = std::sqrt((winWidth / 2.f) * (winWidth / 2.f) + (winHeight / 2.f) * (winHeight / 2.f)); // pythagorus theory
		float dist = cornerDistance + 50.f;

		sf::Vector2f playerPos = m_player.getPosition();

		// Calculate final spawn position
		sf::Vector2f spawnPos = { playerPos.x + std::cos(angle) * dist, playerPos.y + std::sin(angle) * dist };

		m_enemies.emplace_back(spawnPos, playerPos);
	}


	// Enemy logic 
	sf::Vector2f playerPos = m_player.getPosition();
	float playerAngle = m_player.getRotation();

	for (auto& enemy : m_enemies)
	{
		if (!enemy.isActive()) continue;

		enemy.update(dt);
		enemy.updateVisibility(m_player, m_darknessFactor);

		sf::Vector2f enemyPos = enemy.getPosition();

		// Calculate distance between enemy and player
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

	// Implement somse sort of lowpass filter to help smooth things out.


	// Calculate game intensity for audio
	if (!m_debugPauseIntensity)
	{
		// How many enemies are on screen
		float enemyIntensity = (static_cast<float>(m_enemies.size()) / 40.f) * 100.f;
		if (enemyIntensity > 100.f) enemyIntensity = 100.f;

		// Percentage of health left (starting at 50%)
		float healthPanic = 0.f;
		if (m_player.getHealth() < 50.f)
			healthPanic = ((50.f - m_player.getHealth()) / 50.f) * 100.f;

		m_calculatedIntensity = std::max(enemyIntensity, healthPanic);
	}

	float addedIntensity = m_debugIntesityModifier * 10;
	m_intensity = m_calculatedIntensity + addedIntensity;

	// Send values to audio system
	m_wwise.setRTPCValue("Game_Intensity", m_intensity, m_gameAudioID);
	//m_wwise.setRTPCValue("Heartbeat_Intensity", m_player.getHealth(), m_gameAudioID);

	// Update UI
	m_healthBar->update(m_player.getHealth());

	// Remove inactive enemies from vector
	m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(), [](const Enemy& e) { return !e.isActive(); }), m_enemies.end());
}


/// <summary>
/// Check if target angle falls within viewing are from the player.
/// </summary>
/// <returns>True if the enemy is within player view.</returns>
bool DynamicMain::isAngleInView(float enemyAngle, float playerAngle, float fov)
{
	float diff = enemyAngle - playerAngle;

	// Keep angle difference between -180 and 180
	while (diff < -180.f) diff += 360.f;
	while (diff > 180.f) diff -= 360.f;

	return std::abs(diff) <= fov;
}

/// <summary>
/// Draws background, enemies, darkness overlay, player, and UI.
/// </summary>
void DynamicMain::render()
{
	handleResize();

	const float winW = static_cast<float>(m_window->getSize().x);
	const float winH = static_cast<float>(m_window->getSize().y);

	// Setup view based on player's location
	sf::View worldView(m_player.getPosition(), { winW, winH });
	m_window->setView(worldView);

	m_backgroundSprite.setColor(sf::Color::White);
	m_window->draw(m_backgroundSprite);

	sf::Vector2f playerPos = m_player.getPosition();

	float playerAngle = m_player.getRotation();

	// Draw enemies if they are visible
	for (auto& enemy : m_enemies)
	{
		if (!enemy.isActive()) continue;

		sf::Vector2f enemyPos = enemy.getPosition();
		float dx = enemyPos.x - playerPos.x;
		float dy = enemyPos.y - playerPos.y;
		float enemyAngle = std::atan2(dy, dx) * RAD_TO_DEG;

		float distanceSqrd = (dx * dx) + (dy * dy);
		bool inAmbientGlow = distanceSqrd <= (80.f * 80.f);

		float lightRange = m_player.getLightRange();
		bool inTorch = isAngleInView(enemyAngle, playerAngle, VIEW_ANGLE) && distanceSqrd <= (lightRange * lightRange);

		if (m_darknessFactor < 0.95f || inTorch || inAmbientGlow)
			enemy.render(m_window);
	}

	// Switch view for UI elements
	sf::View uiView(sf::FloatRect({ 0.f, 0.f }, { winW, winH }));
	m_window->setView(uiView);

	// Draw darkness and light cuts
	if (m_darknessFactor > 0.01f) 
	{
		std::uint8_t darkAlpha = static_cast<std::uint8_t>(m_darknessFactor * 255);

		m_darknessOverlay.clear(sf::Color(0, 0, 0, darkAlpha));

		// Define how torch light cuts the overlay
		sf::BlendMode cutout(
			sf::BlendMode::Factor::Zero,	// srcColour
			sf::BlendMode::Factor::One,		// dstColour
			sf::BlendMode::Equation::Add,
			sf::BlendMode::Factor::One,		// srcAlpha
			sf::BlendMode::Factor::One,		// dstAlpha
			sf::BlendMode::Equation::ReverseSubtract
		);

		sf::Vector2f screenCentre = { winW / 2, winH / 2 };

		// Generate torch and ambient light shapes
		sf::VertexArray torchCone = buildScreenTorch(screenCentre, playerAngle, m_player.getLightRange(), VIEW_ANGLE, m_darknessFactor);
		sf::VertexArray ambientGlow(sf::PrimitiveType::TriangleFan);
		float ambientRadius = 80.f;
		std::uint8_t centerAlpha = static_cast<std::uint8_t>(255 * m_darknessFactor * 0.8f);
		ambientGlow.append({ screenCentre, sf::Color(0,0,0,centerAlpha) });

		for (int i = 0; i <= 30; i++) 
		{
			float angle = (static_cast<float>(i) / 30.f) * 2.f * PI;
			float x = screenCentre.x + std::cos(angle) * ambientRadius;
			float y = screenCentre.y + std::sin(angle) * ambientRadius;
			ambientGlow.append({ {x,y}, sf::Color(0,0,0,0) }); // Fade to completely dark
		}

		// Apply light shapes to the darkness overlay
		sf::RenderStates renderState;
		renderState.blendMode = cutout;
		m_darknessOverlay.draw(torchCone, renderState);
		m_darknessOverlay.draw(ambientGlow, renderState);
		m_darknessOverlay.display();

		sf::Sprite darkSrpite(m_darknessOverlay.getTexture());
		m_window->draw(darkSrpite);
	}

	// Draw health bar
	m_healthBar->render(m_window);

	// Reset view and draw player
	m_window->setView(worldView);
	m_player.render();

}

/// <summary>
/// Rebuilds the darkness overlay texture when the game window resizes.
/// </summary>
void DynamicMain::handleResize() 
{
	auto size = m_window->getSize();
	if (size == m_darknessOverlay.getSize()) return;

	m_darknessOverlay.resize({ size.x, size.y });
}

/// <summary>
/// Creates the player's toch beam based on angle, range, and darkness.
/// </summary>
/// <returns>The torchlight cone shape.</returns>
sf::VertexArray DynamicMain::buildScreenTorch(sf::Vector2f center, float angleDeg, float range, float halfFOV, float darknessFactor) const
{
	sf::VertexArray cone(sf::PrimitiveType::Triangles);

	// Create multiple rings to fade out light smoothly
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

	// Draw sections for each ring to create a light arc
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

/// <summary>
/// Starts state's audio.
/// </summary>
void DynamicMain::playAudio() 
{
	m_wwise.registerGameObject(m_gameAudioID, "Game BGM Audio");
	m_wwise.postEvent("Play_Game_BGM", m_gameAudioID);
	m_wwise.postEvent("Play_Heartbeat", m_gameAudioID);
}

/// <summary>
/// Stops all state active audio and removes game object from sound engine.
/// </summary>
void DynamicMain::stopAudio() 
{
	m_wwise.stopAll();
	m_wwise.unregisterGameObject(m_gameAudioID);
}