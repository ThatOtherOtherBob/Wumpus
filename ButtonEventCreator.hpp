#pragma once
#include "32blit.hpp"
#include<vector>
#include "Buttons.hpp"

class ButtonEventCreator
{
public:
	std::vector<Buttons> GenerateEvents(blit::ButtonState buttons);
private:
	bool oldButtons[5], newButtons[5] = { false, false, false, false, false };
	void setOldButtons();
	void setNewButtons(blit::ButtonState buttons);
	std::vector<Buttons> createPressedEvents();
};