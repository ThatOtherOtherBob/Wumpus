#include "TextScreenBuffer.hpp"

void TextScreenBuffer::Print(std::string x)
{
	buffer(x);
	postPrintMaintenance();
}

void TextScreenBuffer::postPrintMaintenance()
{
	EnforceKeptLines();

	if (ScrollOnPrint)
		End();

	updateCursorPosition();
}

void TextScreenBuffer::buffer(std::string charactersToBuffer)
{
	std::string lastLine = _lines.back();
	size_t widthOfLastLine = lastLine.size();

	_lines.pop_back();

	auto indent = [&]()
	{
		lastLine = createPadding(_indent);
		if (_indent > 0 && AbsorbSpaceTriggeringWordWrap)
			_justIndented = true;
	};
	auto commitLine = [&]()
	{
		_lines.push_back(lastLine);
		indent();
		widthOfLastLine = _indent;
	};
	auto addSpacesRepresentingTabToBuffer = [&](int spacesToNextTabStop)
	{
		lastLine += createPadding(spacesToNextTabStop);
		widthOfLastLine += spacesToNextTabStop;
		_justIndented = false; // indenting is automatically at the start of every line, tab is when a /t comes in from Print or PrintLine
	};
	auto handleHorizontalTab = [&]()
	{
		if (widthOfLastLine == MaxWidthInChars)
			commitLine();

		int spacesToNextTabStop = HorizontalTabSize - widthOfLastLine % HorizontalTabSize;

		if (widthOfLastLine + spacesToNextTabStop < MaxWidthInChars)
			addSpacesRepresentingTabToBuffer(spacesToNextTabStop);
		else
			commitLine();
	};
	auto handleVerticalTab = [&]()
	{
		for (size_t c = 0; c < VerticalTabSize; c++)
			commitLine();
	};
	auto handleNewLine = [&]()
	{
		if (widthOfLastLine == MaxWidthInChars)
			commitLine();

		commitLine();
	};
	auto addSpaceToBufferAtEndOfLine = [&]()
	{
		commitLine();
		if (AbsorbSpaceTriggeringWordWrap)
			_justIndented = false;
		else
			lastLine += ' ';
	};
	auto addSpaceToBufferInMiddleOfLine = [&]() {
		lastLine += ' ';
		_justIndented = false;
	};
	auto addSpaceToBuffer = [&]()
	{
		if (widthOfLastLine == MaxWidthInChars)
			addSpaceToBufferAtEndOfLine();
		else
			addSpaceToBufferInMiddleOfLine();
	};
	auto handleSpace = [&]()
	{
		addSpaceToBuffer();

		if (++widthOfLastLine == MaxWidthInChars)
			commitLine();
	};
	auto handleOneChar = [&](char newCharacter)
	{
		lastLine += newCharacter;
		widthOfLastLine++;
		_justIndented = false;
	};
	auto commitLastLineBeforeFinalWhitespace = [&](size_t lastWhitespace)
	{
		lastLine.erase(lastWhitespace, MaxWidthInChars);
		commitLine();
	};
	auto commitMultipleCharacters = [&](std::string newCharacters)
	{
		lastLine += newCharacters;
		widthOfLastLine = lastLine.size();
		_justIndented = false;
	};
	auto addCharPastUnbreakableLine = [&](char newCharacter)
	{
		commitLine();
		handleOneChar(newCharacter);
	};
	auto addCharPastBreakableLine = [&](char newCharacter, size_t lastWhitespace)
	{
		std::string textToPushToNextLine = lastLine.substr(lastWhitespace + 1, widthOfLastLine - lastWhitespace);
		commitLastLineBeforeFinalWhitespace(lastWhitespace);
		commitMultipleCharacters(textToPushToNextLine + newCharacter);
	};
	auto handleOneCharTooMany = [&](char newCharacter)
	{
		int lastWhitespace = lastLine.find_last_of(_whitespace);

		if (lastWhitespace == -1 || lastWhitespace + 1 == _indent)
			addCharPastUnbreakableLine(newCharacter);
		else
			addCharPastBreakableLine(newCharacter, lastWhitespace);
	};

	for (char& characterToBuffer : charactersToBuffer)
	{
		if (characterToBuffer == '\t')
			handleHorizontalTab();
		else if (characterToBuffer == '\v')
			handleVerticalTab();
		else if (characterToBuffer == '\r' || characterToBuffer == '\n')
			handleNewLine();
		else if (characterToBuffer == ' ')
			handleSpace();
		else if (widthOfLastLine == MaxWidthInChars)
			handleOneCharTooMany(characterToBuffer);
		else
			handleOneChar(characterToBuffer);
	}

	_lines.push_back(lastLine);
}

size_t TextScreenBuffer::GetWidthInPixels()
{
	return CHARACTER_WIDTH * MaxWidthInChars;
}

size_t TextScreenBuffer::GetHeightInPixels()
{
	return CHARACTER_HEIGHT * MaxHeightInChars;
}

void TextScreenBuffer::RemoveLastLine()
{
	_lines.pop_back();

	updateCursorPosition();
}

void TextScreenBuffer::SetIndent(size_t indent)
{
	_indent = indent;
	Print(createPadding(_indent));
}

void TextScreenBuffer::BlankLastLine()
{
	_lines[_lines.size() - 1] = "";
	_cursorCol = 0;
}

void TextScreenBuffer::PrintLine(std::string x /*= ""*/)
{
	Print(x + "\n");
}

void TextScreenBuffer::PrintLineCentered(std::string x)
{
	printPadded(x, true);
}

void TextScreenBuffer::PrintLineRightAligned(std::string x)
{
	printPadded(x, false);
}

std::string TextScreenBuffer::getLinesWorth(std::string x)
{
	return x.substr(0, MaxWidthInChars);
}

std::string TextScreenBuffer::getFollowingLinesWorth(std::string x)
{
	return x.substr(MaxWidthInChars, x.size());
}

void TextScreenBuffer::printPadded(std::string stringToPrint, bool half)
{
	while (stringToPrint.size() > MaxWidthInChars)
	{
		Print(getLinesWorth(stringToPrint));
		stringToPrint = getFollowingLinesWorth(stringToPrint);
	}

	int neededPadding = (int)((MaxWidthInChars - stringToPrint.size()) / (half ? 2 : 1));

	PrintLine(createPadding(neededPadding) + stringToPrint);
}

std::string TextScreenBuffer::createPadding(size_t needed)
{
	if (needed <= 0)
		return "";

	std::string padding(needed, ' ');

	return padding;
}

void TextScreenBuffer::CLS()
{
	_lines.clear();
	_lines.push_back("");
	TopLine = 0;
	updateCursorPosition();
}

void TextScreenBuffer::ScrollUp(size_t lines /*= 1*/)
{
	if (TopLine < lines)
		TopLine = 0;
	else
		TopLine -= lines;
}

void TextScreenBuffer::ScrollDown(size_t lines /*= 1*/)
{
	int lastPossible = lastSettableLine();
	int lineRequested = TopLine + lines;

	if (lineRequested > lastPossible)
		TopLine = lastPossible;
	else
		TopLine = lineRequested;
}

void TextScreenBuffer::Home()
{
	TopLine = 0;
}

void TextScreenBuffer::End()
{
	TopLine = lastSettableLine();
}

void TextScreenBuffer::PageUp()
{
	ScrollUp(MaxHeightInChars);
}

void TextScreenBuffer::PageDown()
{
	ScrollDown(MaxHeightInChars);
}

size_t TextScreenBuffer::lastSettableLine()
{
	int overflow = _lines.size() - MaxHeightInChars;

	if (overflow < 0)
		return 0;

	return overflow;
}

size_t TextScreenBuffer::countOfRemovableLines()
{
	int countToRemove = _lines.size() - KeptLines;

	if (!ScrollOnPrint && countToRemove > TopLine) // if we've paused scrolling to read
		return TopLine - 1; // remove up to every line before the first visible line

	return countToRemove;
}

void TextScreenBuffer::EnforceKeptLines()
{
	int countToRemove = countOfRemovableLines();

	if (countToRemove > 0)
		removeLines(countToRemove);
}

void TextScreenBuffer::removeLines(size_t countToRemove)
{
	std::vector<std::string>::iterator firstLine = _lines.begin();
	_lines.erase(firstLine, firstLine + countToRemove);
	TopLine -= countToRemove;
}

std::vector<std::string> TextScreenBuffer::GetVisibleLines()
{
	std::vector<std::string>::iterator firstLine = _lines.begin() + TopLine;
	std::vector<std::string>::iterator lastLine = firstLine + lastVisibleLine();
	std::vector<std::string> ret(firstLine, lastLine);

	return ret;
}

size_t TextScreenBuffer::lastVisibleLine()
{
	size_t totalLines = _lines.size();
	
	return (totalLines > MaxHeightInChars) ? MaxHeightInChars : totalLines;
}

bool TextScreenBuffer::CanScrollUp()
{
	return TopLine != 0;
}

bool TextScreenBuffer::CanScrollDown()
{
	return TopLine != lastSettableLine();
}

bool TextScreenBuffer::lineOnScreen(size_t line)
{
	return line > 0 && line < MaxHeightInChars;
}

size_t TextScreenBuffer::ShowCursorOnRow()
{
	size_t line = _cursorRow - TopLine;

	return lineOnScreen(line) ? line : CURSOR_OFFSCREEN;
}

size_t TextScreenBuffer::ShowCursorOnCol()
{
	return _cursorCol;
}

void TextScreenBuffer::updateCursorPosition()
{
	size_t row = _lines.size() - 1;
	size_t col = _lines.back().size();

	if (col == MaxWidthInChars)
	{
		_cursorRow = row + 1;
		_cursorCol = 0;
	}
	else
	{
		_cursorRow = row;
		_cursorCol = col;
	}
}

size_t TextScreenBuffer::RowHeight()
{
	return CHARACTER_HEIGHT;
}