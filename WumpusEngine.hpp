#pragma once
#include "TextScroller.hpp"
#include "ButtonEventCreator.hpp"

class WumpusEngine
{
public:
	void Start();
	void Render();
	void Update();
private:
	enum Objects
	{
		Wumpus,
		Pit1,
		Pit2,
		Bat1,
	    Bat2, // hazards in order bats, pits, wumpus on purpose, order relied on by printHazardWarnings and checkForTroubleInRoom
	    Player,
		First = Wumpus,
		LastWumpus = Wumpus, // add more hazards if you want to make it harder
		LastBat = Bat2, // if you add bats and pits, you only need to add them here, more wumpuses will require more modifications and a decision
		LastPit = Pit2, // on whether all must be shot and whether you get more arrows
		LastHazard = LastBat,
		LastObject = Player
	};
	enum GameOverEnum
	{
		No,
		PlayerWon,
		PlayerLost
	};
	GameOverEnum gameOver = GameOverEnum::No;
	int arrowsRemaining = 5;
	size_t locations[6];
	size_t savedLocations[6]; // used to repeat a game
	size_t routeList[20][3] =
	{
		{1, 4, 7}, {0, 2, 9}, {1, 3, 11}, {2, 4, 13}, {0, 3, 5},
		{4, 6, 14}, {5, 7, 16}, {0, 6, 8}, {7, 9, 17}, {1, 8, 10},
		{9, 11, 18}, {2, 10, 12}, {11, 13, 19}, {3, 12, 14}, {5, 13, 15},
		{14, 16, 19}, {6, 15, 17}, {8, 16, 18}, {10, 17, 19}, {12, 15, 18}
	};
	size_t roomNumberBeingRead;
	size_t arrowPath[5];
	size_t arrowPathLength;
	TextScroller scroller;
	TextScreenBuffer buffer;
	ButtonEventCreator buttonEvents;
	void handleShotMissed();
	void setupSDLGraphicsSystem();
	void resetGameVariables(bool includingLocations);
	void placeObjectsInRandomRooms();
	void setupObjectPositions();
	void saveObjectPositions();
	void resetObjectPositions();
	bool multipleObjectsInSameRoom();
	void displayTitleScreen();
	void askIfUserWantsInstructions();
	void handleDecisionOfWhetherToShowInstructions(size_t result);
	void showInstructions();
	void beginRound();
	void printHazardWarnings();
	void printLocation();
	void printExits();
	void promptForWhereToMove();
	void verifyMovement(size_t room);
	void processMovement(size_t room);
	void beginShootRoutine();
	void saveCountOfRoomsToShootThrough(size_t roomCount);
	void saveRoomNumberToShootInto(size_t room);
	void trackShot();
	void shotTheWumpus();
	void shotYourself();
	void moveWumpus();
	void endRound();
	void checkForTroubleInRoom();
	void fellInPit();
	void grabbedByBat();
	int rollDie(size_t sides);
	int randomRoom();
	int randomPath();
	int randomWumpusAction();
	void askIfUserWantsToMoveOrShoot();
	void startShootOrMove(size_t result);
	void resetAsAsked(size_t result);
	void startOver();
	void goodEnd();
	void badEnd();
	bool isInRoom(size_t room);
	bool canReachRoom(size_t room);
	std::function<void()> prepCallback0(void(WumpusEngine::* callback)());
	std::function<void(size_t)> prepCallback1(void(WumpusEngine::* callback)(size_t x));
};