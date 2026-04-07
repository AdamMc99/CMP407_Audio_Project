#pragma once
#include "BaseMenu.h"

class MainMenu : public BaseMenu {
public:
    MainMenu(sf::Font& font, WwiseWrapper& wwise);

    bool initAudio();
    void playAudio();
    void stopAudio();

private:
    uint64_t m_menuBgmAudioID = 200; 
};