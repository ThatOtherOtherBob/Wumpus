#pragma once
#include <vector>
#include <string>
//#include <functional>

/// <summary>
/// This is not meant to hold text data (though it somewhat can), it is meant to format text data fed in from another source.
/// Tabs sent into this control will be turned into spaces per the field "TabSize".
/// This keeps a record of (some of) the lines passed in, they scroll out after "KeptLines" is exceeded.
/// A virtual "Window" is maintained, with "TopLine" determining the first line shown.
/// No display of the data happens in this class, a TextScroller wraps this class for full-screen display.
/// In the future "Window" classes may do the same.
/// </summary>
class TextScreenBuffer
{
public:
	size_t GetWidthInPixels();
	size_t GetHeightInPixels();
	size_t MaxWidthInChars = 45, MaxHeightInChars = 34; // set for hi-res mode, adjust for low-res or for a smaller window, draw a box around for a pop-up window!

	const int CURSOR_OFFSCREEN = -1;
	void Print(std::string x);
	void PrintLine(std::string x = "");
	void PrintLineCentered(std::string x);
	void PrintLineRightAligned(std::string x);
	void CLS();
	void ScrollUp(size_t lines = 1);
	void ScrollDown(size_t lines = 1);
	void Home();
	void End();
	void PageUp();
	void PageDown();
	size_t KeptLines = 300;
	size_t HorizontalTabSize = 5, VerticalTabSize = 5;
	bool ScrollOnPrint = true;
	size_t TopLine = 0;
	void EnforceKeptLines();
	std::vector<std::string> GetVisibleLines(); // todo: use cursorRow and CursorCol
	bool CanScrollUp();
	bool CanScrollDown();
	size_t ShowCursorOnRow();
	size_t ShowCursorOnCol();
	void RemoveLastLine(); // this exists only so the menu function can be lazy and call PrintLine on everything
	bool AbsorbSpaceTriggeringWordWrap = false; // off for command-line behavior, on for word processing behavior
	void BlankLastLine(); // allows ignoring indent for a line or redrawing a line for command-line / prompting / spinner purposes
	void SetIndent(size_t indent);
	size_t RowHeight();
private:
	const size_t CHARACTER_WIDTH = 7, CHARACTER_HEIGHT = 7;
	bool lineOnScreen(size_t line);
	size_t _cursorRow = 0, _cursorCol = 0;
	bool _justIndented = false;
	size_t _indent = 0;
	const std::string _whitespace = " \t\r\n";
	std::vector<std::string> _lines = { "" };
	size_t lastSettableLine();
	void updateCursorPosition();
	std::string getLinesWorth(std::string x);
	std::string getFollowingLinesWorth(std::string x);
	void printPadded(std::string stringToPrint, bool half);
	std::string createPadding(size_t x);
	void buffer(std::string x);
	void postPrintMaintenance();
	void removeLines(size_t countToRemove);
	size_t countOfRemovableLines();
	size_t lastVisibleLine();
};
