#include "WumpusEngine.hpp"
//#include "game.hpp"
//#include "C:\Users\rdotg\source\repos\32blit-boilerplate\fixed_width_font.hpp"
//#include "resources.hpp"
//#include "engine/menu.hpp"

void WumpusEngine::Start()
{
    setupSDLGraphicsSystem();
    scroller.Buffer = &buffer;

    resetGameVariables(true);

    displayTitleScreen();

    askIfUserWantsInstructions();
}

void WumpusEngine::setupSDLGraphicsSystem()
{
    blit::set_screen_mode(blit::ScreenMode::hires);
    blit::screen.alpha = 255;
    blit::screen.mask = nullptr;
}

void WumpusEngine::resetGameVariables(bool includingLocations)
{
    if (includingLocations)
        setupObjectPositions();
    else
        resetObjectPositions();

    arrowsRemaining = 5;

    gameOver = GameOverEnum::No;
}

void WumpusEngine::setupObjectPositions()
{
    do
        placeObjectsInRandomRooms();
    while (multipleObjectsInSameRoom());

    saveObjectPositions();
}

void WumpusEngine::resetObjectPositions()
{
    for (size_t c = 0; c < 6; c++)
        locations[c] = savedLocations[c];
}

void WumpusEngine::placeObjectsInRandomRooms()
{
    do
        for (int objectNumber = Objects::First; objectNumber <= Objects::LastObject; objectNumber++)
            locations[objectNumber] = randomRoom();
    while (multipleObjectsInSameRoom());
}

void WumpusEngine::saveObjectPositions()
{
    for (int objectNumber = Objects::First; objectNumber <= Objects::LastObject; objectNumber++)
        savedLocations[objectNumber] = locations[objectNumber];
}

bool WumpusEngine::multipleObjectsInSameRoom()
{
    for (int objectNumber1 = Objects::LastObject - 1; objectNumber1 >= Objects::First; objectNumber1--)
        for (int objectNumber2 = objectNumber1 + 1; objectNumber2 <= Objects::LastObject; objectNumber2++)
            if (locations[objectNumber1] == locations[objectNumber2])
                return true;

    return false;
}

void WumpusEngine::displayTitleScreen()
{
    buffer.PrintLineCentered("HUNT THE WUMPUS");
    buffer.PrintLineCentered("BY GREGORY YOB");
    buffer.PrintLineCentered("https://en.wikipedia.org/wiki/Gregory_Yob");
    buffer.Print("\v");
    buffer.PrintLineCentered("PORTED BY LBOB");
    buffer.PrintLineCentered("from");
    buffer.PrintLineCentered("https://github.com/kingsawyer/wumpus/blob/master/wumpus.basic");
    buffer.PrintLine();
    buffer.PrintLineCentered("v1.0 of port");
    buffer.Print("\v");
    buffer.PrintLineCentered("CONTROL WITH D-PAD AND A BUTTON.");
    buffer.Print("\v");
}

void WumpusEngine::askIfUserWantsInstructions()
{
    std::vector<std::string> menu_strings = { "Y", "N" };

    scroller.StartMenu("INSTRUCTIONS (Y-N)", menu_strings, prepCallback1(&WumpusEngine::handleDecisionOfWhetherToShowInstructions));
}

void WumpusEngine::handleDecisionOfWhetherToShowInstructions(size_t result)
{
    const int Y = 0/*, N = 1*/;

    if (result == Y)
    {
        showInstructions();
        scroller.Pause(prepCallback0(&WumpusEngine::beginRound));
    }
    else
        beginRound();
}

// todo: figure this out later to make the code cleaner, or go with defines, or find out if this is a late enough version I can use std::invoke and/or typeDef
std::function<void()> WumpusEngine::prepCallback0(void(WumpusEngine::* callback)())
{
    return std::bind(callback, this);
}

std::function<void(size_t)> WumpusEngine::prepCallback1(void(WumpusEngine::*callback)(size_t x))
{
    return std::bind(callback, this, std::placeholders::_1);
}

void WumpusEngine::showInstructions()
{
    buffer.CLS();
    
    buffer.ScrollOnPrint = false;
    buffer.AbsorbSpaceTriggeringWordWrap = true;
    
    buffer.PrintLine("WELCOME TO 'HUNT THE WUMPUS'");
    buffer.Print("  THE WUMPUS LIVES IN A CAVE OF 20 ROOMS. EACH ROOM HAS 3 TUNNELS LEADING TO OTHER ROOMS. (LOOK AT A DODECAHEDRON TO SEE HOW THIS ");
    buffer.PrintLine("WORKS-IF YOU DON'T KNOW WHAT A DODECAHEDRON IS, ASK SOMEONE)\n");
    
    buffer.SetIndent(5);
    buffer.PrintLine("HAZARDS:");
    buffer.BlankLastLine();
    buffer.PrintLine(" BOTTOMLESS PITS - TWO ROOMS HAVE BOTTOMLESS PITS IN THEM IF YOU GO THERE, YOU FALL INTO THE PIT (& LOSE!)");
    buffer.BlankLastLine(); // don't indent the following header line
    buffer.Print(" SUPER BATS - TWO OTHER ROOMS HAVE SUPER BATS. IF YOU GO THERE, A BAT GRABS YOU AND TAKES YOU TO SOME OTHER ROOM AT RANDOM. (WHICH ");
    buffer.PrintLine("MIGHT BE TROUBLESOME)\n");
    
    buffer.PrintLine("WUMPUS:");
    buffer.BlankLastLine();
    buffer.SetIndent(1);
    buffer.Print("THE WUMPUS IS NOT BOTHERED BY THE HAZARDS (HE HAS SUCKER FEET AND IS TOO BIG FOR A BAT TO LIFT). USUALLY HE IS ASLEEP. TWO THINGS WAKE ");
    buffer.PrintLine("HIM UP: YOUR ENTERING HIS ROOM OR YOUR SHOOTING AN ARROW.");
    buffer.BlankLastLine();
    buffer.Print("     IF THE WUMPUS WAKES, HE MOVES (P=.75) ONE ROOM OR STAYS STILL (P=.25). AFTER THAT, IF HE IS WHERE YOU ARE, HE EATS YOU UP (& YOU ");
    buffer.PrintLine("LOSE!)\n");
    
    buffer.BlankLastLine();
    buffer.PrintLine("     YOU:");
    buffer.PrintLine("EACH TURN YOU MAY MOVE OR SHOOT A CROOKED ARROW");
    buffer.BlankLastLine();
    buffer.SetIndent(3);
    buffer.PrintLine("MOVING: YOU CAN GO ONE ROOM (THRU ONE TUNNEL)");
    buffer.Print("ARROWS: YOU HAVE 5 ARROWS. YOU LOSE WHEN YOU RUN OUT. EACH ARROW CAN GO FROM 1 TO 5 ROOMS. YOU AIM BY TELLING THE COMPUTER THE ROOM#S ");
    buffer.PrintLine("YOU WANT THE ARROW TO GO TO. IF THE ARROW CAN'T GO THAT WAY (IE NO TUNNEL) IT MOVES AT RANDOM TO THE NEXT ROOM.");
    buffer.BlankLastLine();
    buffer.SetIndent(5);
    buffer.PrintLine("IF THE ARROW HITS THE WUMPUS, YOU WIN.");
    buffer.PrintLine("IF THE ARROW HITS YOU, YOU LOSE.\n");
    
    buffer.BlankLastLine();
    buffer.SetIndent(4);
    buffer.PrintLine("WARNINGS:");
    buffer.PrintLine(" WHEN YOU ARE ONE ROOM AWAY FROM WUMPUS OR HAZARD, THE COMPUTER SAYS:");
    buffer.BlankLastLine();
    buffer.SetIndent(1);
    buffer.PrintLine("WUMPUS-  'I SMELL A WUMPUS'");
    buffer.PrintLine("BAT   -  'BATS NEARBY'");
    buffer.PrintLine("PIT   -  'I FEEL A DRAFT'");
    buffer.SetIndent(0);
    buffer.PrintLine();
    buffer.PrintLineCentered("PRESS A TO CONTINUE");
}

void WumpusEngine::beginRound()
{
    buffer.ScrollOnPrint = true;

    buffer.PrintLine();
    printHazardWarnings();
    printLocation();
    printExits();
    
    askIfUserWantsToMoveOrShoot();
}

void WumpusEngine::printHazardWarnings()
{
    for (int hazardNumber = Objects::First; hazardNumber <= Objects::LastHazard; hazardNumber++)
        for (int exit = 0; exit < 3; exit++)
            if (routeList[locations[Objects::Player]][exit] == locations[hazardNumber])
            {
                if (hazardNumber <= Objects::LastWumpus)
                    buffer.PrintLine("I SMELL A WUMPUS!");
                else if (hazardNumber <= Objects::LastPit)
                    buffer.PrintLine("I FEEL A DRAFT");
                else
                    buffer.PrintLine("BATS NEARBY!");
            }
}

void WumpusEngine::printLocation()
{
    buffer.Print("YOU ARE IN ROOM ");
    buffer.PrintLine(std::to_string(locations[Objects::Player] + 1));
}

void WumpusEngine::printExits()
{
    size_t *routes = routeList[locations[Objects::Player]];
    
    buffer.Print("TUNNELS LEAD TO ");
    buffer.Print(std::to_string(routes[0] + 1));
    buffer.Print(" ");
    buffer.Print(std::to_string(routes[1] + 1));
    buffer.Print(" ");
    buffer.Print(std::to_string(routes[2] + 1));
    buffer.PrintLine();
}
    
void WumpusEngine::promptForWhereToMove()
{
    scroller.PromptForNumber("WHERE TO?", 20, prepCallback1(&WumpusEngine::verifyMovement));
}

bool WumpusEngine::isInRoom(size_t room)
{
    return locations[Objects::Player] == room;
}

bool WumpusEngine::canReachRoom(size_t room)
{
    size_t* routes = routeList[locations[Objects::Player]];
    
    for (int routeNumber = 0; routeNumber < 3; routeNumber++)
        if (routes[routeNumber] == room)
            return true;

    return false;
}

void WumpusEngine::verifyMovement(size_t room)
{
    if (isInRoom(--room) || canReachRoom(room))
        processMovement(room);
    else
    {
        buffer.PrintLine("NOT POSSIBLE -");
        scroller.PromptForNumber("WHERE TO?", 20, prepCallback1(&WumpusEngine::verifyMovement));
    }
} // todo: rewrite to not allow bad options and get rid of the need for verification in 1.1.0

void WumpusEngine::processMovement(size_t room)
{
    locations[Objects::Player] = room;
    
    checkForTroubleInRoom();
}

void WumpusEngine::beginShootRoutine()
{
    roomNumberBeingRead = 0;
    scroller.PromptForNumber("NO. OF ROOMS(1-5)?", 5, prepCallback1(&WumpusEngine::saveCountOfRoomsToShootThrough));
}

void WumpusEngine::saveCountOfRoomsToShootThrough(size_t roomCount)
{
    arrowPathLength = roomCount;
    
    scroller.PromptForNumber("ROOM #?", 20, prepCallback1(&WumpusEngine::saveRoomNumberToShootInto));
}

void WumpusEngine::saveRoomNumberToShootInto(size_t room)
{
    room--;
    
    if (roomNumberBeingRead > 1 && arrowPath[roomNumberBeingRead - 2] == room)
    {
        buffer.Print("ARROWS AREN'T THAT CROOKED - TRY ANOTHER ROOM"); // wide enough to not need printline!
        scroller.PromptForNumber("ROOM #?", 20, prepCallback1(&WumpusEngine::saveRoomNumberToShootInto));
        return;
    }
    
    arrowPath[roomNumberBeingRead] = room;
    
    if (++roomNumberBeingRead < arrowPathLength)
        scroller.PromptForNumber("ROOM #?", 20, prepCallback1(&WumpusEngine::saveRoomNumberToShootInto));
    else
        trackShot();
}

void WumpusEngine::trackShot()
{
    int locationOfPlayer = locations[Objects::Player];
    int currentLocationOfArrow = locationOfPlayer;
    int path;
    int locationOfWumpus = locations[Objects::Wumpus];
    const size_t NOT_FOUND = 3;

    for (int flightPathSection = 0; flightPathSection < arrowPathLength; flightPathSection++)
    {
        for (path = 0; path < 3; path++)
            if (routeList[currentLocationOfArrow][path] == arrowPath[flightPathSection])
                break;
    
        currentLocationOfArrow = (path == NOT_FOUND) ? routeList[currentLocationOfArrow][randomPath()] : arrowPath[flightPathSection];

        if (currentLocationOfArrow == locationOfWumpus)
        {
            shotTheWumpus();
            endRound();
            return;
        }
        else if (currentLocationOfArrow == locationOfPlayer)
        {
            shotYourself();
            endRound();
            return;
        }
    }

    handleShotMissed();
}

void WumpusEngine::handleShotMissed()
{
    buffer.PrintLine("MISSED");

    moveWumpus();

    if (--arrowsRemaining == 0)
        gameOver = GameOverEnum::PlayerLost;

    endRound();
}

void WumpusEngine::shotTheWumpus()
{
    buffer.PrintLine("AHA! YOU GOT THE WUMPUS!");
    gameOver = GameOverEnum::PlayerWon;
}

void WumpusEngine::shotYourself()
{
    buffer.PrintLine("OUCH! ARROW GOT YOU!");
    gameOver = GameOverEnum::PlayerLost;
}

void WumpusEngine::moveWumpus()
{
    const int SLEEP = 3;
    
    int moveOrSleep = randomWumpusAction();
    
    if (moveOrSleep != SLEEP)
        locations[Objects::Wumpus] = routeList[locations[Objects::Wumpus]][moveOrSleep];
    
    if (locations[Objects::Wumpus] == locations[Objects::Player])
    {
        buffer.PrintLine("TSK TSK TSK- WUMPUS GOT YOU!");
        gameOver = GameOverEnum::PlayerLost;
    }
}

void WumpusEngine::endRound()
{
    if (gameOver == GameOverEnum::PlayerWon)
        goodEnd();
    else if (gameOver == GameOverEnum::PlayerLost)
        badEnd();
    else
        beginRound();
}

void WumpusEngine::checkForTroubleInRoom()
{
    int playerLocation = locations[Objects::Player];
    
    for (int hazardNumber = Objects::First; hazardNumber <= LastHazard; hazardNumber++)
        if (locations[hazardNumber] == playerLocation)
        {
            if (hazardNumber <= Objects::LastWumpus)
            {
                buffer.PrintLine("...OOPS! BUMPED A WUMPUS!");
                moveWumpus();
            }

            if (hazardNumber >= Objects::Pit1 && hazardNumber <= LastPit)
                fellInPit();
            else if (hazardNumber >= Objects::Bat1 && hazardNumber <= LastBat)
            {
                grabbedByBat();
                return; // we will recurse, so don't call endRound twice
            }

            if (gameOver == GameOverEnum::PlayerLost)
                break;
        }

    endRound();
}

void WumpusEngine::fellInPit()
{
    buffer.PrintLine("YYYIIIIEEEE . . . FELL IN PIT");
    gameOver = GameOverEnum::PlayerLost;
}

void WumpusEngine::grabbedByBat()
{
    buffer.PrintLine("ZAP--SUPER BAT SNATCH! ELSEWHEREVILLE FOR YOU!");
    locations[Objects::Player] = randomRoom();
    checkForTroubleInRoom();
}

int WumpusEngine::rollDie(size_t sides)
{
    return (int)(blit::random() % sides);
}

int WumpusEngine::randomRoom()
{
    const int NUMBER_OF_ROOMS = 20;

    return rollDie(NUMBER_OF_ROOMS);
}

int WumpusEngine::randomPath()
{
    const int NUMBER_OF_PATHS = 3;

    return rollDie(NUMBER_OF_PATHS);
}

int WumpusEngine::randomWumpusAction()
{
    const int NUMBER_OF_PATHS_TO_TAKE_PLUS_STAYING_PUT = 4;

    return rollDie(NUMBER_OF_PATHS_TO_TAKE_PLUS_STAYING_PUT);
}

void WumpusEngine::askIfUserWantsToMoveOrShoot()
{
    std::vector<std::string> options = { "S", "M" };
    
    scroller.StartMenu("SHOOT OR MOVE (S-M)", options, prepCallback1(&WumpusEngine::startShootOrMove));
}

void WumpusEngine::startShootOrMove(size_t result)
{
    const int S = 0/*, M = 1*/;

    if (result == S)
        beginShootRoutine();
    else
        promptForWhereToMove();
}

void WumpusEngine::resetAsAsked(size_t result)
{
    const size_t /*Y = 0, */N = 1;

    resetGameVariables(result == N);
    
    buffer.PrintLine("HUNT THE WUMPUS");
    
    beginRound();
}

void WumpusEngine::startOver()
{
    std::vector<std::string> options = { "Y", "N" };
    
    scroller.StartMenu("SAME SET-UP (Y-N)", options, prepCallback1(&WumpusEngine::resetAsAsked));
}

void WumpusEngine::goodEnd()
{
    buffer.PrintLine("HEE HEE HEE - THE WUMPUS'LL GETCHA NEXT TIME!!");
    startOver();
}

void WumpusEngine::badEnd()
{
    buffer.PrintLine("HA HA HA - YOU LOSE!");
    startOver();
}

void WumpusEngine::Render()
{
    scroller.Render();
}

void WumpusEngine::Update()
{
    scroller.HandleEvents(buttonEvents.GenerateEvents(blit::buttons));
}