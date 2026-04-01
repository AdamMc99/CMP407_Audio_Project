#include "WwiseWrapper.h"

#include <cassert>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>					// Memory Manager interface
#include <AK/SoundEngine/Common/AkMemoryMgrModule.h>			// Default memory manager
#include <AK/SoundEngine/Common/IAkStreamMgr.h>                 // Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h>                    // Thread defines
#include <AK/SoundEngine/Common/AkSoundEngine.h>				// Sound engine
#include <AK/MusicEngine/Common/AkMusicEngine.h>				// Music engine
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>              // Spatial Audio

// Include for communication between Wwise and the game -- Not needed in the release version
#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif // AK_OPTIMIZED

//------------------------------------------------------------------------------
WwiseWrapper::WwiseWrapper() :
	mainListenerID(0)
{

}

//------------------------------------------------------------------------------
bool WwiseWrapper::initSoundEngine(const std::wstring& soundbankPath,
	const std::wstring& language,
	uint64_t listenerID)
{
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert(false && "Could not create the memory manager");
		return false;
	}

	// Create and initialize an instance of the default streaming manager. Note
	// that you can override the default streaming manager with your own. 
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	// Customize the Stream Manager settings here.

	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		return false;
	}

	// Create a streaming device.
	// Note that you can override the default low-level I/O module with your own.
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	// Customize the streaming device settings here.

	// CAkFilePackageLowLevelIODeferred::Init() creates a streaming device
	// in the Stream Manager, and registers itself as the File Location Resolver.
	if (lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		return false;
	}

	// Create the Sound Engine
	// Using default initialization parameters
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}

	//Setup the Music Engine.
	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		return false;
	}

	// Initialize Spatial Audio
	// Using default initialization parameters
	AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 
	if (AK::SpatialAudio::Init(settings) != AK_Success)
	{
		assert(!"Could not initialize the Spatial Audio.");
		return false;
	}

#ifndef AK_OPTIMIZED
	// Initialize communications (not in release build!)
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Could not initialize communication.");
		return false;
	}
#endif // AK_OPTIMIZED

	//Setup soundbank location.
	if (lowLevelIO.SetBasePath(soundbankPath.c_str()) != AK_Success)
	{
		assert(!"Could not set soundbank base path.");
	}
	//Set our localisation language.
	AK::StreamMgr::SetCurrentLanguage(language.c_str());

	//Load the Init bank (there will always be an Init bank).
	AkBankID initBankId;
	if (AK::SoundEngine::LoadBank(AKTEXT("Init.bnk"), initBankId) != AK_Success)
	{
		assert(!"Could not load Init bank.");
	}

	//Initialise our listener.
	mainListenerID = listenerID;
	if (AK::SoundEngine::RegisterGameObj(mainListenerID, "Main Listener") != AK_Success)
	{
		assert(!"Could not register listener game object.");
	}
	if (AK::SoundEngine::SetDefaultListeners(&mainListenerID, 1) != AK_Success)
	{
		assert(!"Could not set default listener to Main Listener Gameobject.");
	}

	return true;
}

void WwiseWrapper::terminateSoundEngine()
{
#ifndef AK_OPTIMIZED
	// Terminate Communication Services
	AK::Comm::Term();
#endif // AK_OPTIMIZED

	//Terminate the music engine.
	AK::MusicEngine::Term();

	// Terminate the sound engine
	AK::SoundEngine::Term();

	// Terminate the streaming device and streaming manager

	// CAkFilePackageLowLevelIODeferred::Term() destroys its associated streaming device 
	// that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
	lowLevelIO.Term();

	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	// Terminate the Memory Manager
	AK::MemoryMgr::Term();
}

void WwiseWrapper::update() 
{
	AK::SoundEngine::RenderAudio();
}

bool WwiseWrapper::loadBank(const std::wstring& bankName)
{
	AkBankID bankID;
	if (AK::SoundEngine::LoadBank(bankName.c_str(), bankID) != AK_Success) 
	{
		assert(!"Could not load sound bank!");
		terminateSoundEngine();
		return false;
	}
	return true;
}

void WwiseWrapper::postEvent(const std::string& eventName, uint64_t gameObjectID) 
{
	AK::SoundEngine::PostEvent(eventName.c_str(), gameObjectID);
}

void WwiseWrapper::setRTCPValue(const std::string& rtcpName, float value, uint64_t gameObjectID) 
{
	AK::SoundEngine::SetRTPCValue(rtcpName.c_str(), value, gameObjectID);
}

void WwiseWrapper::registerGameObject(uint64_t gameObjectID, const std::string& objectName)
{
	AK::SoundEngine::RegisterGameObj(gameObjectID, objectName.c_str());
}

void WwiseWrapper::unregisterGameObject(uint64_t gameObjectID)
{
	AK::SoundEngine::UnregisterGameObj(gameObjectID);
}

void WwiseWrapper::stopAll(uint64_t gameObjectID)
{
	if (gameObjectID == 0) 
	{
		AK::SoundEngine::StopAll();
	}
	else 
	{
		AK::SoundEngine::StopAll(gameObjectID);
	}
}
