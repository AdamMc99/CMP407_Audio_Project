#pragma once

#include "../SoundEngine/Common/AkFilePackageLowLevelIODeferred.h"	//Sample low-level I/O implementation provided by Audiokinetic

#include <cstdint>
#include <string>

///	Convenience class to encapsulate the Wwise initialisation/termination code.
class WwiseWrapper
{
public:
	///	Default constructor, sets our listenerID to 0.
	WwiseWrapper();

	///	Initialises Wwise.
	/*!
		\param soundbankPath The base path to your soundbanks.
		\param language Which language to default to (this must match the
			   language in your Wwise project).
		\param listenerID The unique ID for your project's main listener.

		\return True on success, false on failure.
	 */
	bool initSoundEngine(const std::wstring& soundbankPath,
		const std::wstring& language = AKTEXT("English(US)"),
		uint64_t listenerID = 0);

	///	Cleans up Wwise before we quit the program.
	void terminateSoundEngine();

	// --------- MY STUFF -------------------
	void update();
	bool loadBank(const std::wstring& bankName);
	void postEvent(const std::string& eventName, uint64_t gameObjectID);
	void registerGameObject(uint64_t gameObjectID, const std::string& objectName);
	void unregisterGameObject(uint64_t gameObjectID);
	void stopAll(uint64_t gameObjectID = 0);

private:
	///	Low level I/O implementation for file loading.
	/*!
		We're using the default Low-Level I/O implementation that's part of the
		SDK's sample code, with the file package extension.
	 */
	CAkFilePackageLowLevelIODeferred lowLevelIO;

	///	Our main listener ID.
	uint64_t mainListenerID;
};
