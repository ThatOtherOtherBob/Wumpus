#pragma once
#include "TextScreenBuffer.hpp"
#include "Buttons.hpp"
#include "32blit.hpp"

class TextScroller
{
public:
    TextScreenBuffer *Buffer;
    void HandleEvents(std::vector<Buttons> events);
    void Pause(std::function<void(void)> callback);
    void StartMenu(std::string prompt, std::vector<std::string> items, std::function<void(size_t)> callback, size_t initial = 0);
    void PromptForNumber(std::string prompt, size_t max, std::function<void(size_t)> callback, size_t min = 1, size_t start = 1, size_t jump = 5);
    void Render();
    blit::Pen BackgroundColor = blit::Pen(0, 0, 0), TextColor = blit::Pen(255, 255, 255), ArrowColor = blit::Pen(255, 0, 0);
    //WumpusEngine *engine;
private:
    enum modeEnum
    {
        DoingNothing,
        Paused,
        Menu,
        PickingNumber
    } mode = modeEnum::DoingNothing;
    std::function<void(void)> waitCallback;
    std::function<void(size_t)> resultCallback;
    int menuSelection;
    int numberMin, numberMax, numberJump, numberSelection;
    std::vector<std::string> menuItems;
    void handlePauseEvents(std::vector<Buttons> events);
    void handleMenuEvents(std::vector<Buttons> events);
    void handlePromptForNumberEvents(std::vector<Buttons> events);
    void unpause();
    void menuLeft();
    void menuRight();
    void menuHome();
    void menuEnd();
    void menuSelect();
    void menuRefresh();
    void numberStepUp();
    void numberStepDown();
    void numberJumpUp();
    void numberJumpDown();
    void numberAccept();
    void numberRefresh();
    void cls();
    void writeTextToScreen();
    void drawNeededArrows();
    void drawUpArrow();
    void drawDownArrow();
};