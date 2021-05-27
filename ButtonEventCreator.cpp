#include "ButtonEventCreator.hpp"

std::vector<Buttons> ButtonEventCreator::GenerateEvents(blit::ButtonState buttons)
{
	setOldButtons();

	setNewButtons(buttons);

	return createPressedEvents();
}

void ButtonEventCreator::setOldButtons()
{
	for (size_t c = 0; c < 5; c++)
		oldButtons[c] = newButtons[c];
}

void ButtonEventCreator::setNewButtons(blit::ButtonState buttons)
{
	newButtons[Buttons::Up] = buttons & blit::Button::DPAD_UP;
	newButtons[Buttons::Down] = buttons & blit::Button::DPAD_DOWN;
	newButtons[Buttons::Left] = buttons & blit::Button::DPAD_LEFT;
	newButtons[Buttons::Right] = buttons & blit::Button::DPAD_RIGHT;
	newButtons[Buttons::A] = buttons & blit::Button::A;
}

std::vector<Buttons> ButtonEventCreator::createPressedEvents()
{
	std::vector<Buttons> events;

	for (size_t button = 0; button < 5; button++)
		if (newButtons[button] && !oldButtons[button])
			events.push_back((Buttons)button);

	return events;
}