#pragma once
#include <AK/SoundEngine/Common/AkSoundEngine.h>

class IntensityManager
{
public:
	IntensityManager();
	void reset() { m_intensity = 0.f; }

	void update(float dt, float darknessFactor, int enemyCount, float currentSpawnRate, float playerHealth);
	float getIntensity() const { return m_intensity; }

private:

	float calculateRaw(float darknessFactor, int enemyCount, float currentSpawnRate, float playerHealth);

	float m_intensity = 0.f;

	// Tuning constants
	static constexpr float SMOOTH_SPEED		= 2.f;	// How fast intensity changes
	static constexpr float MAX_ENEMIES		= 20.f; // Amount of enemies considred max threat
	static constexpr float START_SPAWN_RATE = 3.f;	// Matches START_SPAWN_RATE in DynamicMain
	static constexpr float MIN_SPAWN_RATE	= 0.5f; // Matched MIN_SPAWN_RATE in DynamicMain
	static constexpr float MAX_HEALTH		= 100.f;// Players max health
	
	// Weights must add to 1.0
	static constexpr float WEIGHT_DARKNESS		= 0.25f;
	static constexpr float WEIGHT_ENEMY_COUNT	= 0.25f;
	static constexpr float WEIGHT_SPAWN_RATE	= 0.25f;
	static constexpr float WEIGHT_HEALTH		= 0.25f;
};

