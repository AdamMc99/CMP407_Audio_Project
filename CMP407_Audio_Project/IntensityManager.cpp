#include "IntensityManager.h"
#include <algorithm>
#include <cmath>

IntensityManager::IntensityManager() = default;

void IntensityManager::update(float dt, float darknessFactor, int enemyCount, float currentSpawnRate, float playerHealth)
{
	// Don't begin until room is dark
	if (darknessFactor < 0.95f) return;

	float raw = calculateRaw(darknessFactor, enemyCount, currentSpawnRate, playerHealth);

	float speed = (raw > m_intensity) ? SMOOTH_SPEED * 1.5f : SMOOTH_SPEED;
	m_intensity += (raw - m_intensity) * speed * dt;
	m_intensity = std::clamp(m_intensity, 0.f, 1.f);

	// Replace "Intensity" with whatever i name the RTCP in wwise project
	AK::SoundEngine::SetRTPCValue(AKTEXT("Intensity"), m_intensity * 100.f);
}

float IntensityManager::calculateRaw(float darknessFactor, int enemyCount, float currentSpawnRate, float playerHealth)
{
	// Darkness. 0 = day, 1 = full dark
	//float darkScore = darknessFactor;

	// Enemy count. More enemies = more intense
	float enemyScore = std::clamp(static_cast<float>(enemyCount) / MAX_ENEMIES, 0.f, 1.f);

	// Spawn rate. lower rate = faster spawning = more intense
	float spawnScore = 1.f - std::clamp((currentSpawnRate - MIN_SPAWN_RATE) / (START_SPAWN_RATE - MIN_SPAWN_RATE), 0.f, 1.f);

	// Player health. Lower health = more desperate
	float healthScore = 1.f - std::clamp(playerHealth / MAX_HEALTH, 0.f, 1.f);

	// Weighted sum
	// If using darkScore add (darkScore * WEIGHT_DARKNESS)
	float sum = (enemyScore * WEIGHT_ENEMY_COUNT) + (spawnScore * WEIGHT_SPAWN_RATE) + (healthScore * WEIGHT_HEALTH);

	return std::clamp(sum, 0.f, 1.f);
}
