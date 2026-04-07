#pragma once
#include <SFML/Graphics.hpp>
#include "UIElements.h"
#include "BaseMenu.h"
#include "WwiseWrapper.h"
class PauseMenu : public BaseMenu
{
public:
	PauseMenu(sf::Font& font, WwiseWrapper& wwise);
};

