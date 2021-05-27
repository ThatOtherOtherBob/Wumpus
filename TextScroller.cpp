#include "TextScroller.hpp"
#include "FixedWidthFont.hpp"

void TextScroller::HandleEvents(std::vector<Buttons> events)
{
    if (mode == modeEnum::Paused)
        handlePauseEvents(events);
    else if (mode == modeEnum::Menu)
        handleMenuEvents(events);
    else if (mode == modeEnum::PickingNumber)
        handlePromptForNumberEvents(events);
}

void TextScroller::handlePauseEvents(std::vector<Buttons> events)
{
    for (Buttons event : events)
        if (event == Buttons::Up)
            Buffer->ScrollUp();
        else if (event == Buttons::Down)
            Buffer->ScrollDown();
        else if (event == Buttons::Left)
            Buffer->PageUp();
        else if (event == Buttons::Right)
            Buffer->PageDown();
        else
            unpause();
}

void TextScroller::unpause()
{
    mode = modeEnum::DoingNothing;
    waitCallback();
}

void TextScroller::handleMenuEvents(std::vector<Buttons> events)
{
    for (Buttons event : events)
        if (event == Buttons::Up)
            menuHome();
        else if (event == Buttons::Down)
            menuEnd();
        else if (event == Buttons::Left)
            menuLeft();
        else if (event == Buttons::Right)
            menuRight();
        else
            menuSelect();
}

void TextScroller::menuLeft()
{
    if (!menuSelection == 0)
        menuSelection--;

    menuRefresh();
}

void TextScroller::menuRight()
{
    if (menuSelection < menuItems.size() - 1)
        menuSelection++;

    menuRefresh();
}

void TextScroller::menuHome()
{
    menuSelection = 0;

    menuRefresh();
}

void TextScroller::menuEnd()
{
    menuSelection = menuItems.size() - 1;
    
    menuRefresh();
}

void TextScroller::menuSelect()
{
    mode = modeEnum::DoingNothing;
    Buffer->PrintLine();
    resultCallback(menuSelection);
}

void TextScroller::menuRefresh()
{
    Buffer->BlankLastLine();

    for (size_t c = 0; c < menuItems.size(); c++)
    {
        Buffer->Print(menuSelection == c ? "[" : " ");
        Buffer->Print(menuItems[c]);
        Buffer->Print(menuSelection == c ? "]" : " ");
    }
}

void TextScroller::handlePromptForNumberEvents(std::vector<Buttons> events)
{
    for (Buttons event : events)
        if (event == Buttons::Up)
            numberStepUp();
        else if (event == Buttons::Down)
            numberStepDown();
        else if (event == Buttons::Left)
            numberJumpUp();
        else if (event == Buttons::Right)
            numberJumpDown();
        else
            numberAccept();
}

void TextScroller::numberStepUp()
{
    if (numberSelection < numberMax)
        numberSelection++;

    numberRefresh();
}

void TextScroller::numberStepDown()
{
    if (numberSelection > numberMin)
        numberSelection--;

    numberRefresh();
}

void TextScroller::numberJumpUp()
{
    int proposedNumber = numberSelection + numberJump;
    
    numberSelection = (proposedNumber > numberMax) ? numberMax : proposedNumber;

    numberRefresh();
}

void TextScroller::numberJumpDown()
{
    int proposedNumber = numberSelection - numberJump;

    numberSelection = (proposedNumber < numberMin) ? numberMin : proposedNumber;

    numberRefresh();
}

void TextScroller::numberAccept()
{
    mode = modeEnum::DoingNothing;
    Buffer->PrintLine();
    resultCallback(numberSelection);
}

void TextScroller::StartMenu(std::string prompt, std::vector<std::string> items, std::function<void(size_t)> callback, size_t initial /*= 0*/)
{
    mode = modeEnum::Menu;

    Buffer->PrintLine(prompt);

    menuItems = items;
    resultCallback = callback;
    menuSelection = initial;
    
    menuRefresh();
}

void TextScroller::PromptForNumber(std::string prompt, size_t max, std::function<void(size_t)> callback, size_t min /*= 1*/, size_t start /*=1*/, size_t jump /*=5*/)
{
    Buffer->PrintLine(prompt);
    
    mode = modeEnum::PickingNumber;
    numberMin = min;
    numberMax = max;
    numberSelection = start;
    numberJump = jump;
    resultCallback = callback;

    numberRefresh();
}

void TextScroller::numberRefresh()
{
    Buffer->BlankLastLine();
    Buffer->Print(std::to_string(numberSelection));
}

void TextScroller::Pause(std::function<void(void)> callback)
{
    waitCallback = callback;
    mode = modeEnum::Paused;
}

void TextScroller::cls()
{
    blit::screen.pen = BackgroundColor;
    blit::screen.clear();
}

void TextScroller::writeTextToScreen()
{
    size_t offset = 0, rowHeight = Buffer->RowHeight();
    
    blit::screen.pen = TextColor;

    for (auto line : Buffer->GetVisibleLines())
    {
        blit::screen.text(line, FIXED_WIDTH_FONT, blit::Point(0, offset));
        offset += rowHeight;
    }
}

void TextScroller::drawNeededArrows()
{
    blit::screen.pen = ArrowColor;

    if (Buffer->CanScrollUp())
        drawUpArrow();

    if (Buffer->CanScrollDown())
        drawDownArrow();
}

void TextScroller::drawUpArrow()
{
    const blit::Point ARROW_FRONT = blit::Point(315, 1);
    const blit::Point ARROW_BACK = blit::Point(315, 5);
    const blit::Point ARROW_SIDE1 = blit::Point(313, 3);
    const blit::Point ARROW_SIDE2 = blit::Point(317, 3);

    blit::screen.line(ARROW_BACK, ARROW_FRONT);
    blit::screen.line(ARROW_SIDE1, ARROW_FRONT);
    blit::screen.line(ARROW_SIDE2, ARROW_FRONT);
}

void TextScroller::drawDownArrow()
{
    const blit::Point ARROW_FRONT = blit::Point(315, 236);
    const blit::Point ARROW_BACK = blit::Point(315, 232);
    const blit::Point ARROW_SIDE1 = blit::Point(313, 234);
    const blit::Point ARROW_SIDE2 = blit::Point(317, 234);

    blit::screen.line(ARROW_BACK, ARROW_FRONT);
    blit::screen.line(ARROW_SIDE1, ARROW_FRONT);
    blit::screen.line(ARROW_SIDE2, ARROW_FRONT);
}

void TextScroller::Render()
{
    cls();

    writeTextToScreen();

    drawNeededArrows();
}
