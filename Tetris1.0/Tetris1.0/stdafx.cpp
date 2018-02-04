
#include "stdafx.h"

bool Point::operator=(const Point& other) {

	if (other.x == this->x && other.y == this->y)
		return true;

	return false;
}

void GotoXy(int x, int y) {

	COORD pos = { 2 * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

}
